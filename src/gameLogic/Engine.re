open CustomUtils;
open GlobalTypes;

module RestrictedBoard = {
  /** what a player would see */
  type rCell =
    | Hidden
    | Visible(int) // int: num of adjacent mines
    | Flagged;

  type t = matrix(rCell);
  type cellList = list((rCell, coords));

  exception InvalidBoard(string);

  let make = (board: Board.model): t =>
    if (Board.hasVisibleMines(board)) {
      raise(InvalidBoard("the board has visible mines!"));
    } else {
      Matrix.map(
        ~f=
          ({state, numAdjacentMines}: Board.Cell.model, _) => {
            let rCell: rCell =
              switch (state) {
              | Hidden => Hidden
              | Visible => Visible(numAdjacentMines)
              | Flagged => Flagged
              };
            rCell;
          },
        board,
      );
    };
};

/** a number on the board and how it affects where mines are */
module BoardConstraint = {
  type effect =
    | Include
    | Exclude;
  type t = {
    mineCount: int,
    originCoords: Coords.t,
    coordsSet: CoordsSet.t,
    effect,
  };

  let compare = (a, b) =>
    switch (Coords.compare(a.originCoords, b.originCoords)) {
    | 0 => a.effect == b.effect ? 0 : 1
    | num => num
    };

  let exclude = g => {...g, effect: Exclude};

  // let printable = ({mineCount, originCoords, coordsSet, effect}) => {
  //   let (x, y) = originCoords;
  //   [|
  //     "Constraint",
  //     "x: " ++ string_of_int(x),
  //     "y: " ++ string_of_int(y),
  //     "mineCount: " ++ string_of_int(mineCount),
  //     [|"coordsSet: " Coords|]
  //   |];
  // };

  let make = (originCoords, mineCount, board) => {
    let coordsSet = ref(CoordsSet.empty);
    let mineCount = ref(mineCount);
    let (x, y) = originCoords;
    // Js.log([|"Origin: ", string_of_int(x), string_of_int(y)|]);
    // Js.log("board: ");
    // Js.log(board);
    // let adjacent = Matrix.getAdjacentWithCoords(originCoords, board);
    let adjacent =
      Coords.getAdjacent(originCoords, Matrix.size(board))
      |> List.map(((x, y)) => (board[y][x], (x, y)));
    List.iter(
      ((cell: RestrictedBoard.rCell, adjCoord)) =>
        switch (cell) {
        | Hidden =>
          (
            () => {
              coordsSet := CoordsSet.add(adjCoord, coordsSet^);
            }
          )()
        | Visible(_) => ()
        | Flagged => (() => mineCount := mineCount^ - 1)()
        },
      adjacent,
    );

    // Js.logMany([|"adjacent: "|]);
    // Js.log(
    //   Coords.getAdjacent(originCoords, Matrix.size(board)) |> Array.of_list,
    // );
    // Js.log("hidden:");
    // Js.log(coordsSet^ |> CoordsSet.elements |> Array.of_list);

    let const = {
      originCoords,
      coordsSet: coordsSet^,
      mineCount: mineCount^,
      effect: Include,
    };
    Js.log(const);

    const;
  };

  let makeListFromRestrictedBoard = board => {
    let constraints = ref([]);

    board
    |> Matrix.flattenWithCoords
    |> Array.to_list
    |> List.iter(((cell: RestrictedBoard.rCell, coords)) =>
         switch (cell) {
         | Visible(mineCount) =>
           let newConstraint = make(coords, mineCount, board);
           if (CoordsSet.cardinal(newConstraint.coordsSet) > 0) {
             constraints := [newConstraint, ...constraints^];
           };
         | _ => ()
         }
       );

    constraints^;
  };
};

module ConstraintSet = Set.Make(BoardConstraint);

let excludeConstraints = constraints =>
  constraints
  |> ConstraintSet.elements
  |> List.map((c: BoardConstraint.t) => {...c, effect: Exclude})
  |> ConstraintSet.of_list;

/** Groups describe sets of cells with uniforn minimum and maximum possible mines. */
module Group = {
  type t = {
    // minMines should always be less than or equal to maxMines
    minMines: int,
    maxMines: int,
    constraintSet: ConstraintSet.t,
    coordsSet: CoordsSet.t,
  };

  let compare = (a, b) =>
    ConstraintSet.compare(a.constraintSet, b.constraintSet);

  let make = (boardConstraint: BoardConstraint.t) => {
    let {coordsSet, mineCount}: BoardConstraint.t = boardConstraint;
    {
      coordsSet,
      maxMines: mineCount,
      minMines: mineCount,
      constraintSet: ConstraintSet.singleton(boardConstraint),
    };
  };

  let isEmpty = t => CoordsSet.is_empty(t.coordsSet);

  let conflate = (a, b) => {
    open CoordsSet;
    let interCoords = inter(a.coordsSet, b.coordsSet);
    let exclACoords = diff(a.coordsSet, interCoords);
    let exclBCoords = diff(b.coordsSet, interCoords);

    let interGroup = {
      coordsSet: interCoords,
      minMines:
        IntUtils.max([
          a.minMines - cardinal(exclACoords),
          b.minMines - cardinal(exclBCoords),
          0,
        ]),
      maxMines:
        IntUtils.min([cardinal(interCoords), a.maxMines, b.maxMines]),
      constraintSet: ConstraintSet.union(a.constraintSet, b.constraintSet),
    };

    let exclAGroup = {
      coordsSet: exclACoords,
      minMines: a.minMines - interGroup.maxMines,
      maxMines: a.maxMines - interGroup.minMines,
      constraintSet:
        ConstraintSet.union(
          a.constraintSet,
          excludeConstraints(b.constraintSet),
        ),
    };

    let exclBGroup = {
      coordsSet: exclBCoords,
      minMines: b.minMines - interGroup.maxMines,
      maxMines: b.maxMines - interGroup.minMines,
      constraintSet:
        ConstraintSet.union(
          b.constraintSet,
          excludeConstraints(a.constraintSet),
        ),
    };

    (exclAGroup, interGroup, exclBGroup);
  };

  let getActionIfCertain =
      ({coordsSet, minMines, maxMines}): option(Game.action) => {
    let canCheck = maxMines == 0;
    let canFlag = CoordsSet.cardinal(coordsSet) == minMines;
    // Js.log("min mines: " ++ string_of_int(minMines));
    // Js.log("numCells: " ++ string_of_int(CoordsSet.cardinal(coordsSet)));
    let coords = CoordsSet.choose(coordsSet);
    switch (canCheck, canFlag) {
    | (true, _) => Some(Check(coords))
    | (_, true) => Some(ToggleFlag(coords))
    | (false, false) => None
    };
  };
};

module GroupSet = Set.Make(Group);

exception InvalidConnection;

let applyConstraint = (groups, const: BoardConstraint.t) => {
  let constraintGroup = Group.make(const);
  let targetGroup = ref(constraintGroup);
  let toNormalize = ref(groups);
  let normalizedGroups = ref([]);
  let break = ref(false);
  while (! break^) {
    let (unConnected, connected) =
      List.partition(
        (g: Group.t) => {
          let numInter =
            CoordsSet.inter(g.coordsSet, targetGroup^.coordsSet)
            |> CoordsSet.cardinal;
          numInter == 0;
        },
        toNormalize^,
      );
    normalizedGroups := List.concat([normalizedGroups^, unConnected]);

    switch (connected) {
    | [] => break := true
    | [first, ...rest] =>
      let (onlyTarget, inter, onlyFirst) =
        Group.conflate(targetGroup^, first);
      let nonEmpty = [inter, onlyFirst] |> List.filter(Group.isEmpty);
      normalizedGroups := List.concat([normalizedGroups^, nonEmpty]);
      // targetGroup could be empty now, but that would break
      // the while loop next iteration
      targetGroup := onlyTarget;
      toNormalize := rest;
    };
  };
  let normalizedGroups = normalizedGroups^;
  let targetGroup = targetGroup^;
  // Js.log("is empty: " ++ string_of_bool(Group.isEmpty(targetGroup)));
  // Js.log(
  //   "len: " ++ string_of_int(CoordsSet.cardinal(targetGroup.coordsSet)),
  // );
  Group.isEmpty(targetGroup)
    ? normalizedGroups : List.concat([normalizedGroups, [targetGroup]]);
};

type t = RestrictedBoard.t => Game.action;

let getActionFromEngine = (t, unrestrictedBoard) => {
  let restrictedBoard = RestrictedBoard.make(unrestrictedBoard);
  let action: Game.action = t(restrictedBoard);
  switch (action) {
  | Check((x, y)) => Js.log({j|Check: ($x $y)|j})
  | ToggleFlag((x, y)) => Js.log({j|Flag: ($x $y)|j})
  | _ => ()
  };

  action;
};

let random: t =
  board => {
    let (_, coords) =
      board
      |> Matrix.flattenWithCoords
      |> Belt.Array.shuffle
      |> Array.to_list
      |> List.filter(((c: RestrictedBoard.rCell, _)) => c == Hidden)
      |> List.hd;

    Check(coords);
  };

type action =
  | ReportMove(Game.action)
  | ComputeBoard;

let solver = (board: RestrictedBoard.t) => {
  let unAppliedConstraints =
    ref(BoardConstraint.makeListFromRestrictedBoard(board));

  let normalizedGroups = ref([]);
  let action = ref(None);

  while (action^ == None) {
    switch (unAppliedConstraints^) {
    | [] =>
      Js.log("making random move");
      Js.log(normalizedGroups^ |> Array.of_list);
      action := Some(random(board));
    | [firstConstraint, ...rest] =>
      unAppliedConstraints := rest;
      let (x, y) = firstConstraint.originCoords;
      let {mineCount, coordsSet}: BoardConstraint.t = firstConstraint;
      Js.log(
        "adding constraint "
        ++ string_of_int(x)
        ++ " "
        ++ string_of_int(y)
        ++ " ("
        ++ string_of_int(mineCount)
        ++ ")",
      );
      Js.log("cells: ");
      Js.log(coordsSet |> CoordsSet.elements |> Array.of_list);

      normalizedGroups := applyConstraint(normalizedGroups^, firstConstraint);
      action :=
        (
          switch (
            List.find(
              g => Group.getActionIfCertain(g) != None,
              normalizedGroups^,
            )
          ) {
          | exception Not_found => None
          | group =>
            let a = Group.getActionIfCertain(group);
            if (a != None) {
              Js.log("making certain move");
              Js.log(normalizedGroups^ |> Array.of_list);
            } else {
              Js.log("couldn't find move");
              Js.log(normalizedGroups^ |> Array.of_list);
            };
            a;
          }
        );
    };
  };

  switch (action^) {
  | Some(action) => action
  | None => raise(Not_found)
  };
};

type engineEntry = {
  name: string,
  engine: t,
};

type registry = list(engineEntry);
let registry = [
  {name: "solver", engine: solver},
  {name: "random", engine: random},
];