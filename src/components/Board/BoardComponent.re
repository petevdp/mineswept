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
let make =
    (
      ~model: Board.model,
      ~handlers: handlers,
      ~isGameOver: bool,
      ~recommendedMove: option(Game.action),
      ~showOverlay: bool,
    ) => {
  let cellComponents =
    model
    // add clickHandler
    |> Matrix.map(
         ~f=({state, mined, numAdjacentMines}: Board.Cell.model, coords) =>
         (
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
               let recommendedMoveForCell:
                 option(CellComponent.recommendedMoveForCell) =
                 switch (recommendedMove) {
                 | Some(Check(c)) when c == coords => Some(Check)
                 | Some(ToggleFlag(c)) when c == coords => Some(ToggleFlag)
                 | _ => None
                 };

               {
                 state,
                 mined,
                 numAdjacentMines,
                 handleClick,
                 isGameOver,
                 coords,
                 recommendedMoveForCell,
                 showOverlay,
               };
             }: CellComponent.props
           ): CellComponent.props
           // START HERE - test to see if the recommendedMoveForCell works
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
                 recommendedMoveForCell,
                 showOverlay,
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
                   recommendedMoveForCell
                   showOverlay
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