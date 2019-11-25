open GlobalTypes;
open CustomUtils;

type appState = {
  board: Board.model,
  boardActionHandler: BoardComponent.actionHandler,
};

type appAction =
  | BoardAction(Cell.action, coords)
  | RegisterCellActionHandler(BoardComponent.actionHandler);

let initBoard = () => Board.make(~size=(9, 9), ~minedCells=[(0, 0)]);

[@react.component]
let make = () => {
  let ({board, boardActionHandler}, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | BoardAction(cellAction, coords) => {
            ...state,
            board: Board.update((cellAction, coords), state.board),
          }
        | RegisterCellActionHandler(handler) => {
            ...state,
            boardActionHandler: handler,
          }
        },
      {board: initBoard(), boardActionHandler: (_, _) => ()},
    );
  React.useEffect1(
    () => {
      dispatch(
        RegisterCellActionHandler(
          (action, coords) => dispatch(BoardAction(action, coords)),
        ),
      );
      None;
    },
    [||],
  );

  <BoardComponent model=board actionHandler=boardActionHandler />;
};