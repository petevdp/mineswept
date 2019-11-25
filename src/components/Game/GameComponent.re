open GlobalTypes;
open CustomUtils;

type actionHandler = (coords, Cell.action) => unit;

// type getCellProps =
//   (model: Cell.model, ~coords: coords, ~actionHandler: actionHandler, ~minedCells: list(coords)) =>
//   CellComponent.props;

let getCellProps =
    (
      {state, mined, numAdjacentMines}: Board.hydratedCellModel,
      ~coords: coords,
      ~actionHandler: actionHandler,
    )
    : CellComponent.props => {
  let handleClick: CellComponent.handleClick =
    click => {
      let action =
        switch (click) {
        | Left => Cell.Check
        | Right => Cell.ToggleFlag
        };
      actionHandler(coords, action);
    };
  {state, mined, numAdjacentMines, handleClick};
};

[@react.component]
let make = () => {
  let minedCells = [(1, 1)];

  let actionHandler: actionHandler =
    (coords, action) => {
      Js.log("action: ");
      Js.log(coords);
      Js.log(action);
    };

  let board =
    Board.make(~size=(10, 10), ~minedCells)
    |> Matrix.map((model, coords) =>
         getCellProps(model, ~coords, ~actionHandler)
       );

  <BoardComponent cellModelMatrix=board actionHandler />;
};