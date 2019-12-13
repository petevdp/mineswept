open GlobalTypes;
open CustomUtils;

type appState = {
  gameHistory: Game.history,
  engineRegistry: Engine.Registry.t,
  selectedEngineName: String.t,
  playGameOutWithEngine: bool,
  fallbackGameInitOptions: Game.initOptions,
};

type action =
  | NewGame(Game.initOptions)
  | HumanGameAction(Game.action)
  | EngineGameAction
  | PlayGameWithEngine;

let gameSize = 6;
let gameOptions: Game.initOptions = {
  size: (gameSize, gameSize),
  mineCount: gameSize * gameSize / 8,
  minePopulationStrategy: Game.MinePopulationStrategy.random,
};

let startingAppState = {
  gameHistory: [Game.make(gameOptions)],
  selectedEngineName: "naive",
  engineRegistry: Engine.Registry.registry,
  playGameOutWithEngine: false,
  fallbackGameInitOptions: gameOptions,
};

[@react.component]
let make = () => {
  let (appState, dispatch) =
    React.useReducer(
      (prevState: appState, action: action) =>
        switch (action) {
        | NewGame(options) => {
            ...prevState,
            playGameOutWithEngine: false,
            gameHistory: [Game.make(options)],
          }
        | HumanGameAction(action) => {
            ...prevState,
            gameHistory: Game.reduce(prevState.gameHistory, action),
          }
        | EngineGameAction =>
          let {
            gameHistory,
            selectedEngineName,
            engineRegistry,
            fallbackGameInitOptions,
          } = prevState;
          let {engine}: Engine.Registry.registration =
            StrMap.find(selectedEngineName, engineRegistry);
          let getActionFromEngine = Engine.getActionFromEngine(~engine);
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
      startingAppState,
    );
  let {gameHistory, playGameOutWithEngine} = appState;
  let [currGameModel, ..._] = gameHistory;

  let isGameOver =
    switch (currGameModel.phase) {
    | Start
    | Playing => false
    | Ended(_) => true
    };
  React.useEffect3(
    () => {
      if (playGameOutWithEngine && !isGameOver) {
        dispatch(EngineGameAction);
      };
      Some(() => ());
    },
    (gameHistory, playGameOutWithEngine, currGameModel),
  );

  let [currGameModel, ..._] = gameHistory;

  let isGameOver =
    switch (currGameModel.phase) {
    | Start
    | Playing => false
    | Ended(_) => true
    };

  /** set up player action dispatching for the board */
  let boardHandlers: BoardComponent.handlers =
    isGameOver
      // if game is over these handlers won't do anything
      ? {onCheck: _ => (), onFlagToggle: _ => ()}
      : {
        onCheck: coords => dispatch(HumanGameAction(Game.Check(coords))),
        onFlagToggle: coords =>
          dispatch(HumanGameAction(Game.ToggleFlag(coords))),
      };

  let panelGameActionHandlers: ControlPanelComponent.handlers = {
    isGameOver
      ? {
        onRewindGame: _ => (),
        onMakeEngineMove: _ => (),
        onPlayGameWithEngine: _ => (),
      }
      : {
        onRewindGame: steps => dispatch(HumanGameAction(Rewind(steps))),
        onMakeEngineMove: () => dispatch(EngineGameAction),
        onPlayGameWithEngine: () => dispatch(PlayGameWithEngine),
      };
  };

  let onNewGame = () => dispatch(NewGame(gameOptions));

  let {mineCount, flagCount, phase as gamePhase, board}: Game.model = currGameModel;

  // this might be innacurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  <React.Fragment>
    <ControlPanelComponent
      onNewGame
      gamePhase
      minesLeft
      handlers=panelGameActionHandlers
    />
    <BoardComponent model=board handlers=boardHandlers isGameOver />
  </React.Fragment>;
};