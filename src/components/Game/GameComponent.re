open GlobalTypes;
open CustomUtils;

module ControlPanel = {
  type action =
    | Clear;
};

type appState = {board: Board.model};

type appAction =
  | BoardAction(Cell.action, coords)
  | NewGame;

let initBoard = () => Board.make(~size=(9, 9), ~minedCells=[(0, 0)]);

[@react.component]
let make = () => {
  let ({board}, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | BoardAction(cellAction, coords) => {
            ...state,
            board: Board.update((cellAction, coords), state.board),
          }
        | NewGame => {board: initBoard()}
        },
      {board: initBoard()},
    );

  let boardActionHandler = (action, coords) =>
    dispatch(BoardAction(action, coords));
  let onNewGame = () => dispatch(NewGame);

  <React.Fragment>
    <ControlPanelComponent onNewGame />
    <BoardComponent model=board actionHandler=boardActionHandler />
  </React.Fragment>;
};