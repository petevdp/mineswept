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

  let makeMapFromRestrictedBoard = (board: RestrictedBoard.t) => {
    Matrix.reduce(
      ~f=
        (map, cell: RestrictedBoard.rCell, coords) => {
          let size = Matrix.size(board);
          switch (cell) {
          | Visible(0)
          | Hidden
          | Flagged => map
          | Visible(mineCount) =>
            let adjacentCoords = Coords.getAdjacent(size, coords);
            let boardConstraint =
              List.fold_left(
                (t, (x, y)) => {
                  let {mineCount, coordsSet} = t;
                  switch (board[y][x]) {
                  | Hidden => {
                      ...t,
                      coordsSet: CoordsSet.add((x, y), coordsSet),
                    }
                  | Flagged => {...t, mineCount: mineCount - 1}
                  | Visible(_) => t
                  };
                },
                {
                  mineCount,
                  coordsSet: CoordsSet.empty,
                  originCoords: coords,
                  effect: Include,
                },
                adjacentCoords,
              );

            CoordsMap.add(coords, boardConstraint, map);
          };
        },
      CoordsMap.empty,
      board,
    );
  };

  let exclude = g => {...g, effect: Exclude};
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

    (interGroup, exclAGroup, exclBGroup);
  };

  let getActionIfCertain =
      ({coordsSet, minMines, maxMines}): option(Game.action) => {
    let canCheck = maxMines == 0;
    let canFlag = CoordsSet.cardinal(coordsSet) == minMines;
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
      normalizedGroups :=
        List.concat([normalizedGroups^, [inter, onlyFirst]]);
      targetGroup := onlyTarget;
    };
  };

  // this is dumb
  List.concat([normalizedGroups^, [targetGroup^]]);
};

type t = RestrictedBoard.t => Game.action;

let getActionFromEngine = (t, unrestrictedBoard) => {
  let restrictedBoard = RestrictedBoard.make(unrestrictedBoard);
  t(restrictedBoard);
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
    ref(
      BoardConstraint.makeMapFromRestrictedBoard(board)->CoordsMap.bindings,
    );

  let normalizedGroups = ref([]);
  let action = ref(None);

  while (action^ == None) {
    switch (unAppliedConstraints^) {
    | [] => action := Some(random(board))
    | [(_, firstConstraint), ...rest] =>
      unAppliedConstraints := rest;
      normalizedGroups := applyConstraint(normalizedGroups^, firstConstraint);

      // TODO: optimize
      action :=
        (
          switch (
            List.find(
              g => Group.getActionIfCertain(g) != None,
              normalizedGroups^,
            )
          ) {
          | exception Not_found => None
          | group => Group.getActionIfCertain(group)
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