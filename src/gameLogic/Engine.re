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

  /** get coords of hidden cells with at least one adjacent visible cell*/;
};

type t = RestrictedBoard.t => Game.action;

let getActionFromEngine = (~engine: t, board: Board.model): Game.action =>
  board |> RestrictedBoard.make |> engine;

let firstAvailable: t =
  board => {
    let (_, cellCoords) =
      board
      |> Matrix.flattenWithCoords
      |> Array.to_list
      |> List.filter(((cell: RestrictedBoard.rCell, _)) => cell == Hidden)
      |> List.hd;

    Game.Check(cellCoords);
  };

/** Randomly check a cell */
let random: t =
  board => {
    let (_, cellCoords) =
      board
      |> Matrix.flattenWithCoords
      |> Array.to_list
      |> List.filter(((cellState: RestrictedBoard.rCell, _)) =>
           cellState == Hidden
         )
      |> Belt.List.shuffle
      |> List.hd;

    Game.Check(cellCoords);
  };

module Helpers = {
  type t = list(RestrictedBoard.rCell);
  let allHidden = (list: t): bool => {
    !Belt.List.some(list, (cell: RestrictedBoard.rCell) => cell != Hidden);
  };
};

type ordering =
  | AFirst
  | BFirst
  | Equal;

let orderValue = ordering =>
  switch (ordering) {
  | AFirst => 1
  | BFirst => (-1)
  | Equal => 0
  };

let resolveOrdering = orderings =>
  try (List.find(ord => ord !== Equal, orderings)) {
  | Not_found => Equal
  };

/** OrderedType compatible, ordered */
module Group = {
  /** describes the coords and the effect of descriminators that factor into a group, so groups can be uniquly identified */
  module Descriminator = {
    type effect =
      | Included
      | Excluded;
    type t = {
      coords: Coords.t,
      effect,
    };

    let compare = (a, b) =>
      switch (compare(a.coords, b.coords), a.effect, b.effect) {
      | (0, Included, Included) => 0
      | (0, Included, Excluded) => 1
      | (0, Excluded, Included) => (-1)
      | (0, Excluded, Excluded) => 0
      | (order, _, _) => order
      };
  };
  module DescriminatorSet = Set.Make(Descriminator);
  type t = {
    maxMines: int,
    minMines: int,
    coordsSet: CoordsSet.t,
    descriminatorSet: DescriminatorSet.t,
  };

  let makeBase = (coords, boardSize, numMines, board: RestrictedBoard.t) => {
    Coords.getAdjacent(coords, boardSize)
    |> List.fold_left(
         (group, (x, y)) => {
           let {coordsSet, minMines, maxMines} = group;
           switch (board[y][x]) {
           | Visible(_) => group
           | Flagged => {
               ...group,
               minMines: minMines - 1,
               maxMines: maxMines - 1,
             }
           | Hidden => {
               ...group,
               coordsSet: CoordsSet.add((x, y), coordsSet),
             }
           };
         },
         {
           coordsSet: CoordsSet.empty,
           descriminatorSet:
             DescriminatorSet.singleton({coords, effect: Included}),
           minMines: numMines,
           maxMines: numMines,
         },
       );
  };

  let numCoords = t => List.length(CoordsSet.elements(t.coordsSet));

  let avgMines = t =>
    t.maxMines == t.minMines
      ? Some(float_of_int(numCoords(t)) /. float_of_int(numCoords(t)))
      : None;

  let compare = (a: t, b: t) => {
    let lenA = numCoords(a);
    let lenB = numCoords(b);

    let isAFlaggable = lenA == a.minMines;
    let isBFlaggable = lenB == b.minMines;

    let flagOrder =
      switch (isAFlaggable, isBFlaggable) {
      | (true, true) => Equal
      | (true, false) => AFirst
      | (false, true) => BFirst
      | (false, false) => Equal
      };

    let isACheckable = a.minMines == 0;
    let isBCheckable = b.minMines == 0;

    let checkOrder =
      switch (isACheckable, isBCheckable) {
      | (true, true) => Equal
      | (true, false) => AFirst
      | (false, true) => BFirst
      | (false, false) => Equal
      };

    let averageOrder =
      switch (avgMines(a), avgMines(b)) {
      | (Some(numA), Some(numB)) =>
        switch (numA -. numB) {
        | diff when diff > 0.0 => AFirst
        | diff when diff < 0.0 => BFirst
        | _ => Equal
        }
      | (_, _) => Equal
      };

    let priorityOrdering =
      [flagOrder, checkOrder, averageOrder] |> resolveOrdering;

    switch (priorityOrdering) {
    | Equal => CoordsSet.compare(a.coordsSet, b.coordsSet)
    | _ => orderValue(priorityOrdering)
    };
  };
};

module GroupSet = Set.Make(Group);
module CoordsSetMap = Map.Make(CoordsSet);

/** relating cells and groups they belong to */
module GroupedCellMap = {
  /** individual hidden cell and the groups it's included in */
  type t = CoordsMap.t(GroupSet.t);

  /** curry of regular map merge to merge groupsets */
  let merge =
    CoordsMap.merge((_, a, b) =>
      switch (a, b) {
      | (None, Some(group)) => Some(group)
      | (Some(group), None) => Some(group)
      | (Some(a), Some(b)) => Some(GroupSet.union(a, b))
      | (None, None) => None
      }
    );
  /** change groupCOordsMap into map of coords map of groups */
  let invertGroupMap = groups => {
    CoordsSetMap.fold(
      (coordsSet, group, map) => {
        let addMap =
          CoordsSet.fold(
            (coords, map) =>
              CoordsMap.add(coords, GroupSet.singleton(group), map),
            coordsSet,
            CoordsMap.empty,
          );
        merge(addMap, map);
      },
      groups,
      CoordsMap.empty,
    );
  };

  /** get coordMap from visible cells that have a mineCount > 0 */
  let make = (board: RestrictedBoard.t) => {
    let cellList = board |> Matrix.flattenWithCoords |> Array.to_list;
    let size = Matrix.size(board);

    let groups =
      List.fold_left(
        (map, (cell: RestrictedBoard.rCell, coords: coords)) =>
          switch (cell) {
          | Visible(mineCount) when mineCount == 0 => map
          | Hidden
          | Flagged => map
          | Visible(mineCount) =>
            let group = Group.makeBase(coords, size, mineCount, board);
            switch (CoordsSet.choose(group.coordsSet)) {
            | exception Not_found => map
            | _ => CoordsSetMap.add(group.coordsSet, group, map)
            };
          },
        CoordsSetMap.empty,
        cellList,
      );
    invertGroupMap(groups);
  };
};

/** map from descriminator sets to groups they help define */
module GroupComputations = {
  open Group;
  module DescriminatorMap = Map.Make(DescriminatorSet);

  let getIntersectingCellCoords = (a, b) =>
    CoordsSet.inter(a.coordsSet, b.coordsSet);

  let getConstrainedGroups = (a: Group.t, b: Group.t) => {
    let cdsA = a.coordsSet;
    let cdsB = b.coordsSet;
    let cdsInter = CoordsSet.inter(cdsA, cdsB);

    // ()
    let cdsOnlyA = CoordsSet.filter(elt => !CoordsSet.mem(elt, cdsB), cdsA);
    let cdsOnlyB = CoordsSet.filter(elt => !CoordsSet.mem(elt, cdsA), cdsA);

    let minMinesInter =
      [
        a.minMines - CoordsSet.cardinal(cdsOnlyA),
        b.minMines - CoordsSet.cardinal(cdsOnlyB),
      ]
      |> List.fold_left((max, num) => max > num ? max : num, 0);

    let maxMinesInter =
      [a.maxMines, b.maxMines]
      |> List.fold_left(
           (min, num) => min < num ? min : num,
           CoordsSet.cardinal(cdsInter),
         );

    let gOnlyA = {
      coordsSet: cdsOnlyA,
      descriminatorSet:
        DescriminatorSet.(
          filter(
            d =>
              switch (find(d, b.descriminatorSet)) {
              | exception Not_found => true
              | _ => false
              },
            a.descriminatorSet,
          )
        ),
      minMines: a.minMines - minMinesInter,
      maxMines: a.maxMines - maxMinesInter,
    };

    let gOnlyB = {
      coordsSet: cdsOnlyB,
      descriminatorSet:
        DescriminatorSet.(
          filter(
            d =>
              switch (find(d, a.descriminatorSet)) {
              | exception Not_found => true
              | _ => false
              },
            b.descriminatorSet,
          )
        ),
      minMines: b.minMines - minMinesInter,
      maxMines: b.maxMines - maxMinesInter,
    };
    let gInter = {
      coordsSet: cdsInter,
      descriminatorSet:
        DescriminatorSet.union(b.descriminatorSet, a.descriminatorSet),
      minMines: minMinesInter,
      maxMines: maxMinesInter,
    };

    DescriminatorMap.(
      List.fold_left(
        (map, group) => add(group.descriminatorSet, group, map),
        empty,
        [gOnlyA, gOnlyB, gInter],
      )
    );
  };
};

module BaseGroup = {
  type minMax = (float, float);

  /** describes the outut of a computation on a set of mines */
  type t = {
    numMines: int,
    coordsSet: CoordsSet.t,
  };

  let make = (coords, boardSize, numMines, board: RestrictedBoard.t) => {
    Coords.getAdjacent(coords, boardSize)
    |> List.fold_left(
         ({coordsSet, numMines}, (x, y)) =>
           switch (board[y][x]) {
           | Visible(_) => {coordsSet, numMines}
           | Flagged => {coordsSet, numMines: numMines - 1}
           | Hidden => {
               coordsSet: CoordsSet.add((x, y), coordsSet),
               numMines,
             }
           },
         {coordsSet: CoordsSet.empty, numMines},
       );
  };

  /** get groups from visible cells that have a mineCount > 0 */
  let getNumberGroups = (board: RestrictedBoard.t): CoordsMap.t(t) => {
    let cellList = board |> Matrix.flattenWithCoords |> Array.to_list;
    let size = Matrix.size(board);

    List.fold_left(
      (map: CoordsMap.t(t), (cell: RestrictedBoard.rCell, coords: coords)) =>
        switch (cell) {
        | Visible(mineCount) when mineCount == 0 => map
        | Hidden
        | Flagged => map
        | Visible(mineCount) =>
          let group = make(coords, size, mineCount, board);
          switch (CoordsSet.choose(group.coordsSet)) {
          | exception Not_found => map
          | _ => CoordsMap.add(coords, group, map)
          };
        },
      CoordsMap.empty,
      cellList,
    );
  };

  let isFlaggable = (_, {coordsSet, numMines}: t) =>
    List.length(CoordsSet.elements(coordsSet)) == numMines;

  let noMines = (_, {numMines}: t) => numMines == 0;
};

let solver1: t =
  board => {
    let groups = BaseGroup.getNumberGroups(board);

    // Js.log("groups");
    // Js.log(
    //   Array.of_list(
    //     List.map(
    //       ({coordsSet, numMines}: BaseGroup.t) =>
    //         (numMines, coordsSet |> CoordsSet.elements |> Array.of_list),
    //       groups,
    //     ),
    //   ),
    // );

    switch (CoordsMap.(groups |> filter(BaseGroup.isFlaggable) |> choose)) {
    | exception Not_found =>
      switch (CoordsMap.(groups |> filter(BaseGroup.noMines) |> choose)) {
      | exception Not_found =>
        Js.log("random choice");
        random(board);
      | (_, group) =>
        Js.log("found empty cell, checking...");
        Check(CoordsSet.choose(group.coordsSet));
      }
    | (_, group) =>
      Js.log("found flaggable cell, flagging...");
      ToggleFlag(CoordsSet.choose(group.coordsSet));
    };
  };