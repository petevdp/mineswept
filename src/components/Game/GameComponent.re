open GlobalTypes;
open CustomUtils;

let initBoard = (): Board.model =>
  Board.make(~size=(10, 10), ~minedCells=[(0, 0)]);

type appState = {board: Board.model};
[@react.component]
let make = () => {
  let ({board}, dispatch) =
    // game state
    React.useReducer(
      (state, action) =>
        {board: Game.update(action, state.board, ~initBoard)},
      {board: initBoard()},
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