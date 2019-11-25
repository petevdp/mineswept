open GlobalTypes;
include Board;
type cellModelMatrix = matrix(CellComponent.props);

[@react.component]
let make = (~cellModelMatrix: cellModelMatrix) => {
  let cellComponents =
    Array.map(
      row => {
        let cellComponents =
          Array.map(
            ({state, mined, handleClick}: CellComponent.props) =>
              <CellComponent state mined handleClick />,
            row,
          );
        <tr> {React.array(cellComponents)} </tr>;
      },
      cellModelMatrix,
    );

  <table className="board">
    <tbody> {React.array(cellComponents)} </tbody>
  </table>;
};