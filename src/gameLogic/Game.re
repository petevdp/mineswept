open GlobalTypes;

type action =
  | NewGame
  | Check(coords)
  | ToggleFlag(coords);

type actionHandler = action => unit;

let staticCellCheck = (state: Cell.state): Cell.state =>
  switch (state) {
  | Hidden => Visible
  | Flagged => Hidden
  | Visible => Visible
  };

let toggleFlag = (state: Cell.state): Cell.state =>
  switch (state) {
  | Hidden => Flagged
  | Flagged => Hidden
  | Visible => Visible
  };

let update =
    (action: action, board: Board.model, ~initBoard: unit => Board.model)
    : Board.model => {
  switch (action) {
  | NewGame => initBoard()
  | Check((x, y)) =>
    let cell = board[y][x];
    board[y][x] = {...cell, state: staticCellCheck(cell.state)};
    board;
  | ToggleFlag((x, y)) =>
    let cell = board[y][x];
    board[y][x] = {...cell, state: toggleFlag(cell.state)};
    board;
  };
};