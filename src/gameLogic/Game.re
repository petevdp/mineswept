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

let revealAllMines = (board: Board.model): Board.model =>
  Matrix.map(board, ~f=(cell: Board.hydratedCellModel, _) =>
    if (cell.mined) {
      {...cell, state: Visible};
    } else {
      cell;
    }
  );

// Assumed that mined=false. We're modifying board in place.
let rec checkAndReveal = (coords: coords, board: Board.model): Board.model => {
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
      checkAndReveal(coords, board)
    );
  } else {
    board;
  };
};

let cellCheck =
    (~coords: coords, ~gameState: gameState, ~board: Board.model)
    : (Board.model, gameState) => {
  let (x, y) = coords;
  let cell = board[y][x];
  Js.log("checking cell");
  Js.log(coords);
  Js.log(cell);

  switch (gameState, cell.state, cell.mined) {
  | (Ended, _, _) => (board, Ended)
  | (New | Playing, Hidden, true) => (revealAllMines(board), Ended)
  | (New | Playing, Hidden, false) => (
      checkAndReveal(coords, board),
      Playing,
    )
  | (New | Playing, Visible | Flagged, _) => (board, gameState)
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
  Js.log("gamestate: ");
  Js.log(gameState);
  switch (action, gameState) {
  | (NewGame, _) => (initBoard(), New)
  | (Check(coords), Playing | New) => cellCheck(~coords, ~gameState, ~board)
  | (ToggleFlag(coords), Playing | New) => (
      toggleFlag(coords, board),
      Playing,
    )
  // these two should never actually happen given proper input control, included for completeness
  | (Check(_), Ended) => (board, Ended)
  | (ToggleFlag(_), Ended) => (board, Ended)
  };
};