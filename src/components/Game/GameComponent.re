type appState = {
  gameHistory: Game.history,
  engineRegistry: Engine.registry,
  selectedEngineEntry: Engine.engineEntry,
  playGameOutWithEngine: bool,
  fallbackGameInitOptions: Game.initOptions,
  recommendedMove: option(Game.action),
};

type action =
  | NewGame(Game.initOptions)
  | HumanGameAction(Game.action)
  | EngineGameAction
  | PlayGameWithEngine;

let gameSize = 10;
let gameOptions: Game.initOptions = {
  size: (gameSize, gameSize),
  mineCount: gameSize * gameSize / 6,
  minePopulationStrategy: Game.MinePopulationStrategy.random,
};
let initialGameState = Game.make(gameOptions);
let recommendedMove =
  Some(Engine.getActionFromEngine(Engine.random, initialGameState.board));

let startingAppState = {
  gameHistory: [initialGameState],
  engineRegistry: Engine.registry,
  selectedEngineEntry: List.hd(Engine.registry),
  playGameOutWithEngine: false,
  fallbackGameInitOptions: gameOptions,
  recommendedMove,
};

exception NoGameHistory;
exception NoMovePossible;

[@react.component]
let make = () => {
  let (appState, dispatch) =
    React.useReducer(
      (prevState: appState, action: action) => {
        let newState =
          switch (action) {
          | NewGame(options) =>
            let newGame = Game.make(options);
            {
              ...prevState,
              playGameOutWithEngine: false,
              gameHistory: [newGame],
            };
          | HumanGameAction(action) =>
            let gameHistory = Game.reduce(prevState.gameHistory, action);
            {...prevState, gameHistory};
          | EngineGameAction =>
            let {gameHistory, recommendedMove} = prevState;
            let gameHistory =
              switch (gameHistory, recommendedMove) {
              | (_, None) => raise(NoMovePossible)
              | ([], _) => raise(NoGameHistory)
              | (gameHistory, Some(action)) =>
                Game.reduce(gameHistory, action)
              };

            {...prevState, gameHistory};
          | PlayGameWithEngine => {...prevState, playGameOutWithEngine: true}
          };

        let newGameState =
          switch (newState.gameHistory) {
          | [] => raise(NoGameHistory)
          | [newGameState, ..._] => newGameState
          };

        let {selectedEngineEntry} = newState;

        let recommendedMove =
          switch (action, newGameState.phase) {
          | (
              NewGame(_) | EngineGameAction | HumanGameAction(_),
              Start | Playing,
            ) =>
            Some(
              Engine.getActionFromEngine(
                selectedEngineEntry.engine,
                newGameState.board,
              ),
            )
          | (PlayGameWithEngine, _)
          | (_, Ended(_)) => None
          };

        {...newState, recommendedMove};
      },
      startingAppState,
    );
  let {gameHistory, playGameOutWithEngine} = appState;
  let currGameModel =
    switch (gameHistory) {
    | [] => raise(NoGameHistory)
    | [prev, ..._] => prev
    };

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

  let {mineCount, flagCount, phase: gamePhase, board}: Game.model = currGameModel;

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
    <ReactToolTip />
  </React.Fragment>;
};