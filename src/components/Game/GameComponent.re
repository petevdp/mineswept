open GlobalTypes;
open CustomUtils;

type appState = {gameHistory: Game.history};

type action =
  | GameAction(Game.action)
  | NewGame(Game.initOptions);

let gameOptions: Game.initOptions = {
  size: (10, 10),
  mineCount: 25,
  minePopulationStrategy: Game.MinePopulationStrategy.random,
};

[@react.component]
let make = () => {
  let (appState, dispatch) =
    React.useReducer(
      (prevState: appState, action: action) =>
        switch (action) {
        | NewGame(options) => {gameHistory: [Game.make(options)]}
        | GameAction(action) => {
            gameHistory: Game.reduce(prevState.gameHistory, action),
          }
        },
      {gameHistory: [Game.make(gameOptions)]},
    );

  /** set up player action dispatching for the board */
  let boardHandlers: BoardComponent.handlers = {
    onCheck: coords => dispatch(GameAction(Game.Check(coords))),
    onFlagToggle: coords => dispatch(GameAction(Game.ToggleFlag(coords))),
  };

  let {gameHistory} = appState;

  let [currGameModel, ..._] = gameHistory;

  let onNewGame = () => dispatch(NewGame(gameOptions));
  let onRewindGame = steps => dispatch(GameAction(Rewind(steps)));

  let {mineCount, flagCount, phase as gamePhase, board}: Game.model = currGameModel;

  // this might be innacurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  <React.Fragment>
    <ControlPanelComponent onNewGame gamePhase minesLeft onRewindGame />
    <BoardComponent model=board handlers=boardHandlers />
  </React.Fragment>;
};