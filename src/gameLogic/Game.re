open GlobalTypes;
open CustomUtils;

type action =
  | NewGame
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
};

type actionHandler = action => unit;

let staticCellCheck = (state: Cell.state): Cell.state =>
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

  let onlyMinedCellsLeft: bool =
    (
      () => {
        let allCells = board |> Matrix.flatten |> Array.to_list;
        let visibleCells =
          allCells
          |> List.filter(({state}: Board.hydratedCellModel) =>
               state == Visible
             );

        List.length(allCells) - List.length(visibleCells) == mineCount;
      }
    )();

  let phase =
    switch (prevPhase, cell.mined, onlyMinedCellsLeft) {
    | (Ended(endState), _, _) => Ended(endState)
    | (Start | Playing, true, _) => Ended(Loss)
    | (Start | Playing, false, true) => Ended(Win)
    | (Start | Playing, false, false) => Playing
    };
  (board, phase);
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

// produce new game models from actions
let reduce =
    (model, action, ~initBoard: unit => Board.model, ~mineCount: int): model => {
  // only the below variables should be used in computing the next game model
  let {phase, board} = model;

  let (board, phase) =
    switch (action, phase) {
    | (NewGame, _) => (initBoard(), Start)

    // the game might end when the action is Check
    | (Check(coords), Playing | Start) =>
      cellCheck(phase, board, ~mineCount, ~coords)

    | (ToggleFlag(coords), Playing | Start) => (
        toggleFlag(coords, board),
        Playing,
      )

    // this should never actually be called given proper player input control, included for completeness
    | (Check(_) | ToggleFlag(_), Ended(endState)) => (
        board,
        Ended(endState),
      )
    };

  let flagCount =
    board
    |> Matrix.flatten
    |> Array.to_list
    |> List.filter(({state}: Board.hydratedCellModel) => state == Flagged)
    |> List.length;

  {phase, board, mineCount, flagCount};
};

// extended game model with convenient computed info for view layer
type initOptions = {
  size,
  minePopulationStrategy: MinePopulationStrategy.t,
  mineCount: int,
};

let useGame = ({size, minePopulationStrategy, mineCount}: initOptions) => {
  let initBoard = () => {
    Board.make(
      ~size,
      ~minedCoords=minePopulationStrategy(~size, ~mineCount),
    );
  };

  React.useReducer(
    (model: model, action: action) =>
      reduce(model, action, ~initBoard, ~mineCount),
    {board: initBoard(), phase: Start, flagCount: 0, mineCount},
  );
};