open GlobalTypes;
open CustomUtils;

type appState = {
  gameHistory: Game.history,
  selectedEngine: Engine.t,
  playGameOutWithEngine: bool,
  fallbackGameInitOptions: Game.initOptions,
};

type action =
  | NewGame(Game.initOptions)
  | HumanGameAction(Game.action)
  | EngineGameAction(Game.engineAction)
  | PlayGameWithEngine;

let gameOptions: Game.initOptions = {
  size: (4, 4),
  mineCount: 2,
  minePopulationStrategy: Game.MinePopulationStrategy.random,
};

[@react.component]
let make = () => {
  let (appState, dispatch) =
    React.useReducer(
      (prevState: appState, action: action) =>
        switch (action) {
        | NewGame(options) => {
            ...prevState,
            gameHistory: [Game.make(options)],
          }
        | HumanGameAction(action) => {
            ...prevState,
            gameHistory: Game.reduce(prevState.gameHistory, action),
          }
        | EngineGameAction(_) =>
          let {gameHistory, selectedEngine, fallbackGameInitOptions} = prevState;
          let getActionFromEngine =
            Engine.getActionFromEngine(~engine=selectedEngine);
          let gameHistory =
            switch (gameHistory) {
            | [] =>
              let gameState = Game.make(fallbackGameInitOptions);
              Game.reduce(
                [gameState],
                getActionFromEngine(gameState.board),
              );
            | [gameState, ..._] =>
              Game.reduce(gameHistory, getActionFromEngine(gameState.board))
            };
          {...prevState, gameHistory};
        | PlayGameWithEngine => {...prevState, playGameOutWithEngine: true}
        },
      {
        gameHistory: [Game.make(gameOptions)],
        selectedEngine: Engine.firstAvailable,
        playGameOutWithEngine: false,
        fallbackGameInitOptions: gameOptions,
      },
    );

  /** set up player action dispatching for the board */
  let boardHandlers: BoardComponent.handlers = {
    onCheck: coords => dispatch(HumanGameAction(Game.Check(coords))),
    onFlagToggle: coords =>
      dispatch(HumanGameAction(Game.ToggleFlag(coords))),
  };

  let {gameHistory} = appState;

  let [currGameModel, ..._] = gameHistory;

  let onNewGame = () => dispatch(NewGame(gameOptions));
  let onRewindGame = steps => dispatch(HumanGameAction(Rewind(steps)));

  let {mineCount, flagCount, phase as gamePhase, board}: Game.model = currGameModel;

  // this might be innacurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  <React.Fragment>
    <ControlPanelComponent onNewGame gamePhase minesLeft onRewindGame />
    <BoardComponent model=board handlers=boardHandlers />
  </React.Fragment>;
};