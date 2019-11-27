open GlobalTypes;
open CustomUtils;

type appState = {
  // should be maintained by useGame
  gameModel: Game.model,
};

[@react.component]
let make = () => {
  let (gameModel, gameActionDispatch) =
    Game.useGame({
      size: (10, 10),
      minePopulationStrategy: Game.MinePopulationStrategy.random,
      mineCount: 20,
    });

  // setup player action dispatching for the board
  let boardHandlers: BoardComponent.handlers = {
    onCheck: coords => gameActionDispatch(Game.Check(coords)),
    onFlagToggle: coords => gameActionDispatch(Game.ToggleFlag(coords)),
  };

  // control panel dispatch
  let onNewGame = () => gameActionDispatch(NewGame);
  let gamePhase = gameModel.phase;

  <React.Fragment>
    <ControlPanelComponent onNewGame gamePhase />
    <BoardComponent model={gameModel.board} handlers=boardHandlers />
  </React.Fragment>;
};