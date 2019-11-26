open GlobalTypes;
open CustomUtils;

type rawModel = matrix(Cell.model);

module Coords = {
  let adjacentDiff: list(coords) = [
    (1, 1),
    (1, 0),
    (1, (-1)),
    (0, 1),
    (0, (-1)),
    ((-1), 1),
    ((-1), 0),
    ((-1), (-1)),
  ];

  let getAdjacent = ((x, y), (xSize, ySize)) => {
    adjacentDiff
    |> List.map(((xDiff, yDiff)) => (x + xDiff, y + yDiff))
    // filter out of bounds
    |> List.filter(((x, y)) => x >= 0 && x < xSize && y >= 0 && y < ySize);
  };

  type getNumAdjacent = (coords, list(coords)) => int;
  let getNumAdjacent: getNumAdjacent =
    ((x, y), countedCoords) =>
      adjacentDiff
      |> List.map(((xDiff, yDiff)) => (x + xDiff, y + yDiff))
      |> List.filter(coords =>
           List.exists(checked => checked == coords, countedCoords)
         )
      |> List.length;
};
let adjacentCoords = Coords.getAdjacent;

let getAdjacentCells = ((x, y): coords, matrix: matrix('a)) => {
  let xSize = Array.length(matrix[0]);
  let ySize = Array.length(matrix);

  let adjCells =
    Coords.adjacentDiff
    |> List.map(((xDiff, yDiff)) => (x + xDiff, y + yDiff))
    // filter out of bounds
    |> List.filter(((x, y)) => x >= 0 && x < xSize && y >= 0 && y < ySize)
    |> List.map(((x, y)) => matrix[y][x]);

  adjCells;
};

let makeRaw =
    ((xSize, ySize): size, minedCells: list(coords)): matrix(Cell.model) => {
  Array.init(ySize, i =>
    Array.init(
      xSize,
      j => {
        let mined = List.exists(e => e == (j, i), minedCells);
        let model: Cell.model = {state: Cell.Hidden, mined};
        model;
      },
    )
  );
};

type hydratedCellModel = {
  state: Cell.state,
  mined: bool,
  numAdjacentMines: int,
};

type model = matrix(hydratedCellModel);

let make = (~size: size, ~minedCells: list(coords)): model => {
  makeRaw(size, minedCells)
  |> Matrix.map(~f=({state, mined}: Cell.model, coords: coords) =>
       (
         {
           let numAdjacentMines = Coords.getNumAdjacent(coords, minedCells);
           let hydrated: hydratedCellModel = {state, mined, numAdjacentMines};
           hydrated;
         }: hydratedCellModel
       )
     );
};

let initRandom = (~size: size, ~mineCount: int): model => {
  let (x, y) = size;
  let allCoords = MyList.combinationRange(x, y);
  let shuffled: list(coords) = allCoords->Belt.List.shuffle;
  let minedCells = shuffled->Belt.List.take(mineCount);
  Js.log("mined: ");
  switch (minedCells) {
  | Some(arr) =>
    Js.log(Array.of_list(arr));
    make(~size, ~minedCells=arr);
  | None => make(~size, ~minedCells=[])
  };
};