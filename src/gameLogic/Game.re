open GlobalTypes;
open CustomUtils;

type action =
  // board, minecount
  | Check(coords)
  | ToggleFlag(coords)
  | Rewind(int);

type engineAction =
  | Check(coords)
  | ToggleFlag(coords);

type endState =
  | Win
  | Loss;

type phase =
  | Start
  | Playing
  | Ended(endState);

type model = {
  phase,
  board: Board.model,
  flagCount: int,
  mineCount: int,
  lastAction: option(action),
};

type history = list(model);

type actionHandler = action => unit;

let staticCellCheck = (state: Board.Cell.state): Board.Cell.state =>
  switch (state) {
  | Hidden => Visible
  | Flagged => Hidden
  | Visible => Visible
  };

let cellCheck =
    (
      prevPhase: phase,
      prevBoard: Board.model,
      ~mineCount: int,
      ~coords: coords,
    )
    : (Board.model, phase) => {
  let (x, y) = coords;
  let cell = prevBoard[y][x];

  let board =
    switch (cell.state, cell.mined) {
    | (Hidden, false) => Board.checkAndReveal(coords, prevBoard)
    | (Hidden, true) => Board.revealAllMines(prevBoard)
    | (Flagged, _) => prevBoard

    // the game should be over if this is matched
    | (Visible, _) => prevBoard
    };

  let allCells = board |> Matrix.flatten |> Array.to_list;

  let hasHiddenCells: bool =
    List.exists(({state}: Board.Cell.model) => state == Hidden, allCells);

  let hasIncorrectFlaggedCells: bool =
    List.exists(
      ({state, mined}: Board.Cell.model) => !mined && state == Flagged,
      allCells,
    );

  let hasUnfinishedCells = hasHiddenCells || hasIncorrectFlaggedCells;

  let phase =
    switch (prevPhase, cell.mined, hasUnfinishedCells) {
    // if the game is already over, it's still over
    | (Ended(endState), _, _) => Ended(endState)
    // if you click on a mine, you lose
    | (Start | Playing, true, _) => Ended(Loss)
    // if there are no unfinished cells, you win
    | (Start | Playing, false, false) => Ended(Win)
    // if there still are unfinished cells, you have to keep playing
    | (Start | Playing, false, true) => Playing
    };
  (board, phase);
};

let toggleFlag = ((x, y): coords, board: Board.model): Board.model => {
  let cell: Board.Cell.model = board[y][x];
  let newState: Board.Cell.state =
    switch (cell.state) {
    | Hidden => Flagged
    | Flagged => Hidden
    | Visible => Visible
    };
  board[y][x] = {...cell, state: newState};
  board;
};

module MinePopulationStrategy = {
  // produce a list of coordinates to place mines at
  type t = (~size: size, ~mineCount: int) => list(coords);
  let random: t =
    (~size, ~mineCount) => {
      let (x, y) = size;

      let allCoords = MyList.combinationRange(x, y);

      // Shuffle the list of coords and take the first `mineCount` elements.
      // Belt.List.take outputs option(list('a))
      let minedCoords: option(list(coords)) =
        allCoords->Belt.List.shuffle->Belt.List.take(mineCount);
      switch (minedCoords) {
      | Some(list) => list
      | None => []
      };
    };
};

/* produce new game models from actions **/
let reduce = (history: history, action: action): history => {
  // only the below variables should be used in computing the next game model
  let prevBoard = List.hd(history);
  let {phase as prevPhase, board as prevBoard, mineCount} = prevBoard;

  // make deep copy of board
  let newBoard =
    Array.map(
      (row: array(Board.Cell.model)) => Array.copy(row),
      prevBoard,
    );

  let (newBoard, newPhase) =
    switch (action, phase) {
    | (Rewind(steps), _) =>
      // -1 because the first entry in history is the initial state
      let length = List.length(history) - 1;
      let steps = length > steps ? steps : length;
      Js.log(string_of_int(steps) ++ " steps");
      let {board, phase} = List.nth(history, steps);
      (board, phase);

    // the game might end when the action is Check
    | (Check(coords), Playing | Start) =>
      cellCheck(phase, newBoard, ~mineCount, ~coords)

    | (ToggleFlag(coords), Playing | Start) => (
        toggleFlag(coords, board),
        Playing,
      )

    // this should never actually be called given proper player input control, included for completeness
    | (Check(_) | ToggleFlag(_), Ended(endState)) => (
        newBoard,
        Ended(endState),
      )
    };

  let flagCount =
    newBoard
    |> Matrix.flatten
    |> Array.to_list
    |> List.filter(({state}: Board.Cell.model) => state == Flagged)
    |> List.length;

  let history = [
    {
      phase: newPhase,
      board: newBoard,
      mineCount,
      flagCount,
      lastAction: Some(action),
    },
    ...history,
  ];

  Js.log(List.length(history));
  history;
};

// extended game model with convenient computed info for view layer
type initOptions = {
  size,
  minePopulationStrategy: MinePopulationStrategy.t,
  mineCount: int,
};

let make = ({size, minePopulationStrategy, mineCount}: initOptions) => {
  {
    board:
      Board.make(
        ~size,
        ~minedCoords=minePopulationStrategy(~size, ~mineCount),
      ),
    phase: Start,
    flagCount: 0,
    mineCount,
    lastAction: None,
  };
};