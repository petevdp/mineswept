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
  type t = {
    mineCount: int,
    originCoords: Coords.t,
    coordsSet: CoordsSet.t,
  };

  let compare = (a, b) => Coords.compare(a.originCoords, b.originCoords);

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
                {mineCount, coordsSet: CoordsSet.empty, originCoords: coords},
                adjacentCoords,
              );

            CoordsMap.add(coords, boardConstraint, map);
          };
        },
      CoordsMap.empty,
      board,
    );
  };
};

module ConstraintSet = Set.Make(BoardConstraint);

/** Groups describe sets of cells with uniforn minimum and maximum possible mines. */
module Group = {
  type t = {
    // minMines should always be less than or equal to maxMines
    minMines: int,
    maxMines: int,
    coordsSet: CoordsSet.t,
  };

  let compare = (a, b) => CoordsSet.compare(a.coordsSet, b.coordsSet);

  let make = boardConstraint => {
    let {coordsSet, mineCount}: BoardConstraint.t = boardConstraint;
    {coordsSet, maxMines: mineCount, minMines: mineCount};
  };

  let conflate = (a, b): list(t) => {
    open CoordsSet;
    let interCoords = inter(a.coordsSet, b.coordsSet);
    let exclACoords = filter(e => mem(e, b.coordsSet), a.coordsSet);
    let exclBCoords = filter(e => mem(e, a.coordsSet), b.coordsSet);

    let interGroup = {
      coordsSet: interCoords,
      minMines:
        IntUtils.max([
          a.minMines - cardinal(exclACoords),
          b.minMines - cardinal(exclBCoords),
        ]),
      maxMines:
        IntUtils.min([cardinal(interCoords), a.maxMines, b.maxMines]),
    };

    let exclAGroup = {
      coordsSet: exclACoords,
      minMines: b.minMines - interGroup.maxMines,
      maxMines: b.maxMines - interGroup.minMines,
    };

    let exclBGroup = {
      coordsSet: exclBCoords,
      minMines: b.minMines - interGroup.maxMines,
      maxMines: b.maxMines - interGroup.minMines,
    };

    [interGroup, exclAGroup, exclBGroup];
  };
};

let rec addGroup = (groups: CoordsMap.t(Group.t), groupToInclude: Group.t) => {
  ();
};

let includeGroup = (groups: CoordsMap.t(Group.t), groupToInclude: Group.t) => {
  let fromNewGroups = ref(CoordsSet.empty);
  CoordsSet.fold(
    (coord, groups: CoordsMap.t(Group.t)) =>
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