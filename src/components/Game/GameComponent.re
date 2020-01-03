type appState = {
  gameHistory: Game.history,
  engineRegistry: Engine.registry,
  selectedEngineEntry: Engine.engineEntry,
  playGameOutWithEngine: bool,
  fallbackGameInitOptions: Game.initOptions,
  recommendedMove: option(Game.action),
  showOverlay: bool,
};

type action =
  | NewGame(Game.initOptions)
  | HumanGameAction(Game.action)
  | EngineGameAction
  | PlayGameWithEngine
  | ToggleOverlay;

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
  showOverlay: true,
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
          | ToggleOverlay => {
              ...prevState,
              showOverlay: !prevState.showOverlay,
            }
          };

        let newGameState =
          switch (newState.gameHistory) {
          | [] => raise(NoGameHistory)
          | [newGameState, ..._] => newGameState
          };

        let {selectedEngineEntry} = newState;

        /**
         *  We're essentialy sorting things implicitly by the gamestate, so maybe we should
         *  compare directly against that when deciding to update the recommendedMove
         */
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
          | (PlayGameWithEngine | ToggleOverlay, _) =>
            prevState.recommendedMove
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
        onToggleOverlay: () => dispatch(ToggleOverlay),
      }
      : {
        onRewindGame: steps => dispatch(HumanGameAction(Rewind(steps))),
        onMakeEngineMove: () => dispatch(EngineGameAction),
        onPlayGameWithEngine: () => dispatch(PlayGameWithEngine),
        onToggleOverlay: () => dispatch(ToggleOverlay),
      };
  };

  let onNewGame = () => dispatch(NewGame(gameOptions));

  let {recommendedMove, showOverlay} = appState;
  let {mineCount, flagCount, phase: gamePhase, board}: Game.model = currGameModel;

  // this might be inaccurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  <React.Fragment>
    <ControlPanelComponent
      onNewGame
      gamePhase
      minesLeft
      handlers=panelGameActionHandlers
    />
    <BoardComponent
      model=board
      handlers=boardHandlers
      isGameOver
      recommendedMove
      showOverlay
    />
    <ReactToolTip />
  </React.Fragment>;
};