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

type restrictedBoardInfo = {
  board: RestrictedBoard.t,
  totalNumMines: int,
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

  let print = ({mineCount, originCoords, coordsSet, effect}) => {
    Js.log(
      {j|Constraint: $originCoords, mineCount: $mineCount, effect: $effect|j},
    );
    Js.log(coordsSet |> CoordsSet.elements |> Array.of_list);
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

    let const = {
      originCoords,
      coordsSet: coordsSet^,
      mineCount: mineCount^,
      effect: Include,
    };

    const;
  };

  let makeListFromRestrictedBoard = board => {
    let constraints = ref([]);

    let cellList = board |> Matrix.flattenWithCoords |> Array.to_list;

    cellList
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
    coordsSet: CoordsSet.t,
  };

  let make = (boardConstraint: BoardConstraint.t) => {
    let {coordsSet, mineCount}: BoardConstraint.t = boardConstraint;
    {coordsSet, maxMines: mineCount, minMines: mineCount};
  };

  let isEmpty = t => CoordsSet.is_empty(t.coordsSet);
  let print = ({coordsSet, maxMines, minMines}) => {
    Js.log("Group:");
    Js.log([|
      "max/min: ",
      string_of_int(maxMines),
      string_of_int(minMines),
    |]);
    Js.log(coordsSet |> CoordsSet.elements |> Array.of_list);
  };

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
    };

    let exclAGroup = {
      coordsSet: exclACoords,
      minMines: IntUtils.max([a.minMines - interGroup.maxMines, 0]),
      maxMines: IntUtils.max([a.maxMines - interGroup.minMines, 0]),
    };

    let exclBGroup = {
      coordsSet: exclBCoords,
      minMines: IntUtils.max([b.minMines - interGroup.maxMines, 0]),
      maxMines: IntUtils.max([b.maxMines - interGroup.minMines, 0]),
    };

    (exclAGroup, interGroup, exclBGroup);
  };

  let getActionIfCertain =
      ({coordsSet, minMines, maxMines}): option(GameModel.action) => {
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

type engineAnalysis = {
  recommendedMove: GameModel.action,
  groupMap: CoordsMap.t(Group.t),
};

type engineOutput =
  | Analysis(engineAnalysis)
  | RecommendedMove(GameModel.action);

type t = restrictedBoardInfo => engineOutput;

let getOutputFromEngine = (totalNumMines, t, board): engineOutput => {
  let board = RestrictedBoard.make(board);
  t({board, totalNumMines});
};

exception InvalidConnection;

let applyConstraint = (groups, const: BoardConstraint.t) => {
  Js.log("applying constraint");
  BoardConstraint.print(const);
  let constraintGroup = Group.make(const);
  let targetGroup = ref(constraintGroup);
  let toNormalize = ref(groups);
  let normalizedGroups = ref([]);
  let break = ref(false);

  Group.print(targetGroup^);
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
      let nonEmpty =
        [inter, onlyFirst] |> List.filter(g => !Group.isEmpty(g));
      normalizedGroups := List.concat([normalizedGroups^, nonEmpty]);
      // targetGroup could be empty now, but that would break
      // the while loop next iteration
      targetGroup := onlyTarget;
      toNormalize := rest;
    };
    Group.print(targetGroup^);
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

let random: t =
  ({board}) => {
    let (_, coords) =
      board
      |> Matrix.flattenWithCoords
      |> Belt.Array.shuffle
      |> Array.to_list
      |> List.filter(((c: RestrictedBoard.rCell, _)) => c == Hidden)
      |> List.hd;

    RecommendedMove(Check(coords));
  };

let solver = restrictedBoardInfo => {
  let {board, totalNumMines} = restrictedBoardInfo;
  let unAppliedConstraints =
    ref(BoardConstraint.makeListFromRestrictedBoard(board));

  let cellList = Matrix.flattenWithCoords(board) |> Array.to_list;

  let fullCellSet =
    cellList
    |> List.filter(((cell: RestrictedBoard.rCell, _)) => cell == Hidden)
    |> List.map(((_, coords): (RestrictedBoard.rCell, Coords.t)) => coords)
    |> CoordsSet.of_list;

  let numFlaggedMines =
    cellList
    |> List.filter(((cell: RestrictedBoard.rCell, _)) => cell == Flagged)
    |> List.length;

  let numMinesLeft = totalNumMines - numFlaggedMines;

  let fullBoardGroup: Group.t = {
    minMines: numMinesLeft,
    maxMines: numMinesLeft,
    coordsSet: fullCellSet,
  };

  let normalizedGroups = ref([fullBoardGroup]);

  let action = ref(None);

  while (action^ == None) {
    switch (unAppliedConstraints^) {
    | [] =>
      Js.log(normalizedGroups^ |> Array.of_list);

      action :=
        (
          switch (random(restrictedBoardInfo)) {
          | RecommendedMove(action) => Some(action)
          | _ => raise(Not_found)
          }
        );

    | [firstConstraint, ...rest] =>
      unAppliedConstraints := rest;

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
              Js.log(normalizedGroups^);
            } else {
              Js.log("couldn't find move");
              Js.log(normalizedGroups^);
            };
            a;
          }
        );
    };
  };

  let groupMap = ref(CoordsMap.empty);

  List.iter(
    (group: Group.t) => {
      CoordsSet.iter(
        coord => {groupMap := CoordsMap.add(coord, group, groupMap^)},
        group.coordsSet,
      )
    },
    normalizedGroups^,
  );

  switch (action^) {
  | Some(action) => Analysis({groupMap: groupMap^, recommendedMove: action})
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
