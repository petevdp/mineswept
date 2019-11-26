open GlobalTypes;
open CustomUtils;

let initBoard = (): Board.model =>
  Board.make(~size=(10, 10), ~minedCells=[(0, 0)]);

type appState = {
  board: Board.model,
  gameState: Game.gameState,
};

[@react.component]
let make = () => {
  let ({board}, dispatch) =
    // game state
    React.useReducer(
      (state, action) => {
        let (board, gameState) =
          Game.update(
            action,
            ~board=state.board,
            ~initBoard,
            ~gameState=state.gameState,
          );
        {board, gameState};
      },
      {board: initBoard(), gameState: New},
    );

  // setup player action dispatching for the board
  let boardHandlers: BoardComponent.handlers = {
    onCheck: coords => dispatch(Game.Check(coords)),
    onFlagToggle: coords => dispatch(Game.ToggleFlag(coords)),
  };

  let onNewGame = () => dispatch(NewGame);

  <React.Fragment>
    <ControlPanelComponent onNewGame />
    <BoardComponent model=board handlers=boardHandlers />
  </React.Fragment>;
};