open CustomUtils;

type appState = {
  gameHistory: Game.history,
  engineRegistry: Engine.registry,
  selectedEngineEntry: Engine.engineEntry,
  playGameOutWithEngine: bool,
  fallbackGameInitOptions: Game.initOptions,
  showOverlay: bool,
  engineOutput: option(Engine.engineOutput),
};

type action =
  | NewGame(Game.initOptions)
  | HumanGameAction(GameModel.action)
  | EngineGameAction
  | PlayGameWithEngine
  | ToggleOverlay;

let gameSize = 20;
let mineCount = gameSize * gameSize / 6;

let gameOptions: Game.initOptions = {
  size: (gameSize, gameSize),
  mineCount,
  minePopulationStrategy: Game.MinePopulationStrategy.random,
};

let getOutputFromEngineWithSize = Engine.getOutputFromEngine(mineCount);

let initialGameState = Game.make(gameOptions);

let engineOutput =
  Some(getOutputFromEngineWithSize(Engine.solver, initialGameState.board));

let startingAppState = {
  gameHistory: [initialGameState],
  engineRegistry: Engine.registry,
  selectedEngineEntry: List.hd(Engine.registry),
  playGameOutWithEngine: false,
  fallbackGameInitOptions: gameOptions,
  showOverlay: true,
  engineOutput,
};

let reducer = (prevState: appState, action: action) => {
  let newState =
    switch (action) {
    | NewGame(options) =>
      let {selectedEngineEntry} = prevState;
      let newGame = Game.make(options);

      let engineOutput =
        Some(
          getOutputFromEngineWithSize(
            selectedEngineEntry.engine,
            newGame.board,
          ),
        );

      {
        ...prevState,
        playGameOutWithEngine: false,
        gameHistory: [newGame],
        engineOutput,
      };
    | HumanGameAction(action) =>
      let {gameHistory, selectedEngineEntry} = prevState;

      let newGameHistory = Game.reduce(gameHistory, action);
      let currentGameState = List.hd(newGameHistory);
      let {phase, board}: Game.model = currentGameState;

      let engineOutput =
        switch (phase) {
        | Start
        | Playing =>
          Some(
            getOutputFromEngineWithSize(selectedEngineEntry.engine, board),
          )
        | Ended(_) => None
        };

      {...prevState, gameHistory: newGameHistory, engineOutput};
    | EngineGameAction =>
      let {engineOutput, selectedEngineEntry, gameHistory} = prevState;

      let action =
        switch (engineOutput) {
        | None => raise(Not_found)
        | Some(Analysis({recommendedMove})) => recommendedMove
        | Some(RecommendedMove(recommendedMove)) => recommendedMove
        };

      let newGameHistory = Game.reduce(gameHistory, action);

      let currentGameState = List.hd(newGameHistory);

      let {phase, board}: Game.model = currentGameState;

      let engineOutput =
        switch (phase) {
        | Start
        | Playing =>
          Some(
            getOutputFromEngineWithSize(selectedEngineEntry.engine, board),
          )
        | Ended(_) => None
        };

      let {playGameOutWithEngine} = prevState;

      let playGameOutWithEngine =
        switch (playGameOutWithEngine, phase) {
        | (true, Ended(_)) => false
        | (_, _) => playGameOutWithEngine
        };

      {
        ...prevState,
        engineOutput,
        gameHistory: newGameHistory,
        playGameOutWithEngine,
      };
    | PlayGameWithEngine => {...prevState, playGameOutWithEngine: true}
    | ToggleOverlay => {...prevState, showOverlay: !prevState.showOverlay}
    };

  newState;
};

exception NoGameHistory;
exception NoMovePossible;

[@react.component]
let make = () => {
  let (appState, dispatch) = React.useReducer(reducer, startingAppState);

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
        onCheck: coords =>
          dispatch(HumanGameAction(GameModel.Check(coords))),
        onFlagToggle: coords =>
          dispatch(HumanGameAction(GameModel.ToggleFlag(coords))),
      };

  let panelGameActionHandlers: ControlPanelComponent.handlers = {
    isGameOver
      ? {
        onRewindGame: steps => dispatch(HumanGameAction(Rewind(steps))),
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

  let {engineOutput, showOverlay} = appState;
  let {mineCount, flagCount, phase: gamePhase, board}: Game.model = currGameModel;

  // this might be inaccurate since the player might misplace a flag
  let minesLeft = mineCount - flagCount;

  let (groupMap, recommendedMove) =
    switch (engineOutput) {
    | Some(Analysis({groupMap, recommendedMove})) => (
        groupMap,
        Some(recommendedMove),
      )
    | Some(RecommendedMove(move)) => (CoordsMap.empty, Some(move))
    | _ => (CoordsMap.empty, None)
    };

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
      groupMap
    />
    <ReactToolTip />
  </React.Fragment>;
};
