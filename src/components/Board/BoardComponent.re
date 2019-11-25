open GlobalTypes;
open Board;

type cellModelMatrix = matrix(CellComponent.props);
type actionHandler = (coords, Cell.action) => unit;

module Style = {
  open Css;
  let board = style([]);
};

[@react.component]
let make = (~cellModelMatrix: cellModelMatrix, ~actionHandler: actionHandler) => {
  let cellComponents =
    Array.mapi(
      (i, modelRow) => {
        let cellComponents =
          Array.mapi(
            (
              j,
              {state, mined, numAdjacentMines, handleClick}: CellComponent.props,
            ) =>
              <th key={string_of_int(j)}>
                <CellComponent state mined numAdjacentMines handleClick />
              </th>,
            modelRow,
          );
        <tr key={string_of_int(i)}> {React.array(cellComponents)} </tr>;
      },
      cellModelMatrix,
    );

  <table className=Style.board>
    <tbody> {React.array(cellComponents)} </tbody>
  </table>;
};