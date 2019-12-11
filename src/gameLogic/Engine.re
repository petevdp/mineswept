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

/**
 * how should I model this
 *
 * gcell => cell + list of groups
 * what do I do if to mine chances are different?
 */
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
  // let avgMineChance = ({numMines, allCoords}: t) =>
  //   float_of_int(numMines) /. float_of_int(List.length(allCoords));
  // let getMinMaxMineChance = (groupList: list(t)) => {
  //   Belt.List.reduce(
  //     groupList,
  //     (infinity, neg_infinity),
  //     ((min, max), group: t) => {
  //       let groupMineChance = avgMineChance(group);
  //       (
  //         groupMineChance < min ? groupMineChance : min,
  //         groupMineChance > max ? groupMineChance : max,
  //       );
  //     },
  //   );
  // };
};

// let getSafeActions: t =
//   board => {
//     let cellList = board |> Matrix.flattenWithCoords |> Array.to_list;
//     let visibleWithAdjacent =
//       cellList |> List.filter(((cell: rCell, _)) => {
//         switch (cell) {
//           | Visible(_) => true
//           | _ => false
//         };
//       }) |> List.map(((cell, coords)) => )
//     ();
//   };

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