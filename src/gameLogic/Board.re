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

  /** what a player would see */
  type restrictedModel =
    | Hidden
    | Visible(int) // int: num of adjacent mines
    | Flagged;

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
type restrictedModel = matrix(Cell.restrictedModel);

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

/** get the board with only the info the player sees (for engines). The board
 *  state needs to be a valid playable one, meaning no visible mined cells
 *  or an exception will be thrown.
 */
let getRestrictedModel = (board: model): restrictedModel => {
  Matrix.map(
    ~f=
      ({state, mined, numAdjacentMines}: Cell.model, _) => {
        let restrictedModel: Cell.restrictedModel =
          switch (state, mined) {
          | (Hidden, _) => Hidden
          | (Visible, false) => Visible(numAdjacentMines)
          | (Visible, true) =>
            raise(
              InvalidBoardState(
                "the board has a visible cell which has a mine, so it shouldn't be evaluated by an engine",
              ),
            )
          | (Flagged, _) => Flagged
          };
        restrictedModel;
      },
    board,
  );
};

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