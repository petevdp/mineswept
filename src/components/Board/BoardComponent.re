open GlobalTypes;
open CustomUtils;

type cellModelMatrix = matrix(CellComponent.props);

module Style = {
  open Css;
  let board = style([]);
};

type handlers = {
  onCheck: coords => unit,
  onFlagToggle: coords => unit,
};

[@react.component]
let make = (~model: Board.model, ~handlers: handlers, ~isGameOver: bool) => {
  let cellComponents =
    model
    // add clickHandler
    |> Matrix.map(
         ~f=({state, mined, numAdjacentMines}: Board.Cell.model, coords) =>
         (
           {
             let handleClick = click => {
               let {onCheck, onFlagToggle} = handlers;
               switch ((click: CellComponent.click)) {
               | Left => onCheck(coords)
               | Right => onFlagToggle(coords)
               };
               ();
             };
             {
               state,
               mined,
               numAdjacentMines,
               handleClick,
               isGameOver,
               coords,
             };
           }: CellComponent.props
         )
       )
    // construct tables
    |> Array.mapi((y, modelRow) => {
         let cellComponents =
           Array.mapi(
             (
               x,
               {
                 state,
                 mined,
                 numAdjacentMines,
                 handleClick,
                 isGameOver,
                 coords,
               }: CellComponent.props,
             ) =>
               <th key={string_of_int(x)}>
                 <CellComponent
                   state
                   mined
                   numAdjacentMines
                   handleClick
                   isGameOver
                   coords
                 />
               </th>,
             modelRow,
           );
         <tr key={string_of_int(y)}> {React.array(cellComponents)} </tr>;
       });
  // swallow this default behaviour so we don't get right clicks in between the cells
  let onContextMenu = e => ReactEvent.Mouse.preventDefault(e);

  <React.Fragment>
    <table className=Style.board onContextMenu>
      <tbody> {React.array(cellComponents)} </tbody>
    </table>
  </React.Fragment>;
};