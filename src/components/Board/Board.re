type cellRow = array(Cell.model);
type cellModelMatrix = array(cellRow);

[@react.component]
let make = (~cellModelMatrix as m: cellModelMatrix) => {
  let rows =
    Array.map(
      row => {
        let cells =
          Array.map(cellModel => <td> <Cell model=cellModel /> </td>, row);
        <tr> {React.array(cells)} </tr>;
      },
      m,
    );
  // let cells = Array.make_matrix(4, 4, <Cell state=Cell.Hidden />);
  <table className="board"> <tbody> {React.array(rows)} </tbody> </table>;
};