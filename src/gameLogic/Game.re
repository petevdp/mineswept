open GlobalTypes;
open CustomUtils;

type action =
  | NewGame
  | Check(coords)
  | ToggleFlag(coords);

type gameState =
  | New
  | Playing
  | Ended;

type actionHandler = action => unit;

let staticCellCheck = (state: Cell.state): Cell.state =>
  switch (state) {
  | Hidden => Visible
  | Flagged => Hidden
  | Visible => Visible
  };

// Assumed that mined=false. We're modifying board in place.
let rec revealCells = (coords: coords, board: Board.model): Board.model => {
  let (x, y) = coords;
  let cell = board[y][x];
  board[y][x] = {...cell, state: Cell.Visible};

  // If there a no adjacent mines, then we can reveal the surrounding area.
  if (cell.numAdjacentMines == 0) {
    let board = board;
    let adjacentCoords = Board.adjacentCoords(coords, Matrix.size(board));

    let hiddenAdjacentCoords =
      adjacentCoords |> List.filter(((x, y)) => board[y][x].state === Hidden);
    // recurse into all adjacent coordinates
    hiddenAdjacentCoords->Belt.List.reduce(board, (board, coords) =>
      revealCells(coords, board)
    );
  } else {
    board;
  };
};

let cellCheck =
    (~coords: coords, ~gameState: gameState, ~board: Board.model)
    : (Board.model, gameState) => {
  let (y, x) = coords;
  let cell = board[y][x];
  switch (gameState, cell.state, cell.mined) {
  | (Ended, _, _) => (board, Ended)
  | (New | Playing, Hidden, true) => (board, Ended)
  | (New | Playing, Hidden, false) => (revealCells(coords, board), Playing)
  | (New | Playing, Visible, true | false) => (board, gameState)
  };
};

let toggleFlag = ((x, y): coords, board: Board.model): Board.model => {
  let cell: Board.hydratedCellModel = board[y][x];
  let newState: Cell.state =
    switch (cell.state) {
    | Hidden => Flagged
    | Flagged => Hidden
    | Visible => Visible
    };
  board[y][x] = {...cell, state: newState};
  board;
};

let update =
    (
      action: action,
      ~board: Board.model,
      ~gameState: gameState,
      ~initBoard: unit => Board.model,
    )
    : (Board.model, gameState) => {
  switch (action) {
  | NewGame => (initBoard(), New)
  | Check(coords) => cellCheck(~coords, ~gameState, ~board)
  | ToggleFlag(coords) => (toggleFlag(coords, board), Playing)
  };
};