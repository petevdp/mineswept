open CustomUtils;
open Engine;

/** Represents the effects of a visible number. */
module Constraint = {
  type t = {
    coords: Coords.t,
    mineCount: int,
    effectedCells: CoordsSet.t,
  };

  let compare = (a, b) => Coords.compare(a.coords, b.coords);
  let sharedCells = (a, b) =>
    CoordsSet.inter(a.effectedCells, b.effectedCells);

  let make = (originCoords, startingMineCount, board: RestrictedBoard.t) => {
    Coords.getAdjacent(originCoords, Matrix.size(board))
    |> List.fold_left(
         (c: t, coords: Coords.t) => {
           let (x, y) = coords;
           switch (board[y][x]) {
           | Hidden => {
               ...c,
               effectedCells: CoordsSet.add(coords, c.effectedCells),
             }
           | Visible(_) => c
           | Flagged => {...c, mineCount: c.mineCount - 1}
           };
         },
         {
           coords: originCoords,
           mineCount: startingMineCount,
           effectedCells: CoordsSet.empty,
         },
       );
  };
};

module ConstraintSet = Set.Make(Constraint);
module Constraints = {
  /** Produce the included and excluded pair of constrains for a visible cell */
  exception ConstraintWithNoCoords;

  /** add a constraint and all resulting valid compound constraints (where there are intersecting cells) */;
  let addConstraint = (cellConstraintToAdd: Constraint.t, map) => {
    let map =
      CoordsSetMap.fold(
        (keyCoordsSet, cellConstraintSet, map) => {
          let intersectingCoords =
            CoordsSet.inter(keyCoordsSet, cellConstraintToAdd.effectedCells);
          if (CoordsSet.is_empty(intersectingCoords)) {
            map;
          } else {
            // if a matching coordsset is already present, add to that one
            let existingConstraints =
              try (CoordsSetMap.find(intersectingCoords, map)) {
              | Not_found => ConstraintSet.empty
              };

            let newConstraints =
              ConstraintSet.add(cellConstraintToAdd, existingConstraints);
            CoordsSetMap.add(intersectingCoords, newConstraints, map);
          };
        },
        map,
        CoordsSetMap.empty,
      );
    CoordsSetMap.add(
      cellConstraintToAdd.effectedCells,
      ConstraintSet.singleton(cellConstraintToAdd),
      map,
    );
  };

  /** take a set of constraints, find sets of common cells between one or more of them, and group them together using their coordsets as keys */;
  let compoundConstraints = set =>
    ConstraintSet.fold(addConstraint, set, CoordsSetMap.empty);

  /** The result of applying one or more constraints to a set of cells */;
};

module Group = {
  type t = {
    effectedCells: CoordsSet.t,
    averageMineCount: float,
  };
};