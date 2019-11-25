open GlobalTypes;
open CustomUtils;

type cellModelMatrix = matrix(CellComponent.props);
type actionHandler = (Cell.action, coords) => unit;

module Style = {
  open Css;
  let board = style([]);
};

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
      actionHandler(action, coords);
    };
  {state, mined, numAdjacentMines, handleClick};
};

let getCellClickHandler =
    (coords: coords, actionHandler: actionHandler, click: CellComponent.click) => {
  let action =
    switch (click) {
    | Left => Cell.Check
    | Right => Cell.ToggleFlag
    };
  actionHandler(action, coords);
};

[@react.component]
let make = (~model: Board.model, ~actionHandler: actionHandler) => {
  let cellComponents =
    model
    // add clickHandler
    |> Matrix.map(
         ({state, mined, numAdjacentMines}: Board.hydratedCellModel, coords) =>
         (
           {
             let handleClick = getCellClickHandler(coords, actionHandler);
             {state, mined, numAdjacentMines, handleClick};
           }: CellComponent.props
         )
       )
    // construct tables
    |> Array.mapi((y, modelRow) => {
         let cellComponents =
           Array.mapi(
             (
               x,
               {state, mined, numAdjacentMines, handleClick}: CellComponent.props,
             ) =>
               <th key={string_of_int(x)}>
                 <CellComponent state mined numAdjacentMines handleClick />
               </th>,
             modelRow,
           );
         <tr key={string_of_int(y)}> {React.array(cellComponents)} </tr>;
       });

  <table className=Style.board>
    <tbody> {React.array(cellComponents)} </tbody>
  </table>;
};