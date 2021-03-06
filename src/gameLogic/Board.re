open GlobalTypes;
open CustomUtils;

module Cell = {
  type state =
    | Hidden
    | Visible
    | Flagged;

  /** cell with no computed data */
  type rawModel = {
    state,
    mined: bool,
  };

  type model = {
    state,
    mined: bool,
    numAdjacentMines: int,
  };

  type action =
    | Check
    | ToggleFlag;
};

type model = matrix(Cell.model);

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
    ((xSize, ySize): size, minedCoords: list(coords))
    : matrix(Cell.rawModel) => {
  Array.init(ySize, i =>
    Array.init(
      xSize,
      j => {
        let mined = List.exists(e => e == (j, i), minedCoords);
        let model: Cell.rawModel = {state: Cell.Hidden, mined};
        model;
      },
    )
  );
};

let make = (~size: size, ~minedCoords: list(coords)): model => {
  makeRaw(size, minedCoords)
  |> Matrix.map(~f=({state, mined}: Cell.rawModel, coords: coords) =>
       (
         {
           let numAdjacentMines = Coords.getNumAdjacent(coords, minedCoords);
           {state, mined, numAdjacentMines};
         }: Cell.model
       )
     );
};

exception InvalidBoardState(string);

let revealAllMines = (board: model): model =>
  Matrix.map(board, ~f=(cell: Cell.model, _) =>
    if (cell.mined) {
      {...cell, state: Visible};
    } else {
      cell;
    }
  );

/** Assumed that mined=false. We're modifying board in place. */
let rec checkAndReveal = (coords: coords, board: model): model => {
  let (x, y) = coords;
  let cell = board[y][x];
  board[y][x] = {...cell, state: Cell.Visible};

  // If there a no adjacent mines, then we can reveal the surrounding area.
  if (cell.numAdjacentMines == 0) {
    let board = board;
    let adjacentCoords = Coords.getAdjacent(coords, Matrix.size(board));

    let hiddenAdjacentCoords =
      adjacentCoords |> List.filter(((x, y)) => board[y][x].state === Hidden);
    // recurse into all adjacent coordinates
    hiddenAdjacentCoords->Belt.List.reduce(board, (board, coords) =>
      checkAndReveal(coords, board)
    );
  } else {
    board;
  };
};

let hasUnfinishedCells = model => {
  let cellList = Matrix.toList(model);
  let hasHiddenCells: bool =
    List.exists(({state}: Cell.model) => state == Hidden, cellList);

  let hasIncorrectFlaggedCells: bool =
    List.exists(
      ({state, mined}: Cell.model) => !mined && state == Flagged,
      cellList,
    );

  hasHiddenCells || hasIncorrectFlaggedCells;
};

let hasVisibleMines = model =>
  model
  ->Matrix.toList
  ->Belt.List.some(({state, mined}: Cell.model) =>
      switch (state, mined) {
      | (Visible, true) => true
      | (_, _) => false
      }
    );