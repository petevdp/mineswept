open GlobalTypes;
open CustomUtils;

type appState = {gameHistory: Game.history};

type action =
  | GameAction(Game.action)
  | NewGame(Game.initOptions);

[@react.component]
let make = () => {
  let (appState, dispatch) =
    React.useReducer(
      (prevState: appState, action: action): appState => {
        let gameHistory =
          switch (action) {
          | NewGame(options) => [Game.make(options)]
          | GameAction(action) => Game.reduce(prevState.gameHistory, action)
          };

        let state = {
          gameHistory;
        };
        state;
      },
      {gameHistory: [Game.make(options)]},
    );

  /** set up player action dispatching for the board */
  let boardHandlers: BoardComponent.handlers = {
    onCheck: coords => dispatch(GameAction(Game.Check(coords))),
    onFlagToggle: coords => dispatch(GameAction(Game.ToggleFlag(coords))),
  };

  let onNewGame = () => gameActionDispatch(NewGame);
  let gamePhase = gameModel.phase;

  let {mineCount, flagCount}: Game.model = gameModel;

  // this might be innacurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  <React.Fragment>
    <ControlPanelComponent onNewGame gamePhase minesLeft />
    <BoardComponent model={gameModel.board} handlers=boardHandlers />
  </React.Fragment>;
};