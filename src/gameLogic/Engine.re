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

  let make = boardConstraint => {
    let {coordsSet, mineCount}: BoardConstraint.t = boardConstraint;
    {
      coordsSet,
      maxMines: mineCount,
      minMines: mineCount,
      constraintSet: ConstraintSet.singleton(boardConstraint),
    };
  };

  let conflate = (a, b): list(t) => {
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

    [interGroup, exclAGroup, exclBGroup];
  };

  let mergeAll = (list: list(t)) => {};
};

module GroupSet = Set.Make(Group);

exception InvalidConnection;
/**
 * find a connection in a groups mapped based on effected cells.
 * Will raise Not_found if no connections exist
 */
let findConnection = (groups: CoordsMap.t(GroupSet.t)) => {
  let (coord, connectedGroups) =
    groups
    |> CoordsMap.bindings
    |> List.find(((coords, groupSet)) => GroupSet.cardinal(groupSet) > 1);

  switch (GroupSet.elements(connectedGroups)) {
  | []
  | [_] => raise(InvalidConnection)
  | [a, b, ...rest] => (a, b)
  };
};

let replaceGroups = (toReplace: GroupSet.t, toAdd: GroupSet.t, map) => {
  GroupSet.fold(
    (group: Group.t, map: CoordsMap.t(GroupSet.t)) =>
      CoordsSet.fold(
        (coord, map) => {
          let groupSet = CoordsMap.find(coord, map);
          let toReplaceRemoved =
            GroupSet.diff(groupSet, GroupSet.inter(toReplace, groupSet));
          let addedGroup = GroupSet.add(group, toReplaceRemoved);

          CoordsMap.add(coord, addedGroup, map);
        },
        group.coordsSet,
        map,
      ),
    toAdd,
    map,
  );
};

/**
 *
 */
let rec conflateAll = (groupMap: CoordsMap.t(GroupSet.t)) =>
  switch (findConnection(groupMap)) {
  // if there are no eonnctions between cells, we're done
  | exception Not_found => groupMap
  | (a, b) =>
    let groupMapWithReplacements =
      replaceGroups(
        [a, b] |> GroupSet.of_list,
        Group.conflate(a, b) |> GroupSet.of_list,
      );
    conflateAll(groupMapWithReplacements);
  };

let includeGroup = (groups: CoordsMap.t(Group.t), groupToInclude: Group.t) => {
  let fromNewGroups = ref(CoordsSet.empty);
  CoordsSet.fold(
    (coord, groups: CoordsMap.t(GroupSet.t)) =>
      CoordsSet.mem(coord, fromNewGroups^)
        ? groups
        // there won't be any overlapping coords among these new groups
        : {
          let newGroups =
            switch (CoordsMap.find(coord, groups)) {
            | exception Not_found => [groupToInclude]
            | group => Group.conflate(groupToInclude, group)
            };
          fromNewGroups :=
            newGroups
            |> List.fold_left(
                 (acc, elt: Group.t) => CoordsSet.add(elt, acc),
                 fromNewGroups^,
               );

          // replace the old groups with the new ones
          List.fold_left(
            (groups, group: Group.t) =>
              CoordsSet.fold(
                (coords, groups) => CoordsMap.add(coords, group, groups),
                group.coordsSet,
                groups,
              ),
            groups,
            newGroups,
          );
        },
    groupToInclude.coordsSet,
    groups,
  );
};

type t = RestrictedBoard.t => Game.action;

let computeBoard = ();

type action =
  | ReportMove(Game.action)
  | ComputeBoard;

let solve = (board: RestrictedBoard.t) => {
  let constraints = ref(BoardConstraint.makeMapFromRestrictedBoard(board));
  let groups = ref(CoordsMap.empty);
  let break = ref(false);
  ();
};