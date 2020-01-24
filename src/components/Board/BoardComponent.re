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

type hoveredCellChange =
  | Remove(coords)
  | Add(coords);

let reduceHoveredCells = (hoveredCells, hoverChange) => {
  switch (hoverChange) {
  | Remove(coords) => CoordsSet.remove(coords, hoveredCells)
  | Add(coords) => CoordsSet.add(coords, hoveredCells)
  };
};

[@react.component]
let make =
    (
      ~model: Board.model,
      ~handlers: handlers,
      ~isGameOver: bool,
      ~recommendedMove: option(GameModel.action),
      ~showOverlay: bool,
      ~groupMap: CoordsMap.t(Engine.Group.t),
    ) => {
  let (hoveredCells, dispatchHoveredCellChange) =
    React.useReducer(reduceHoveredCells, CoordsSet.empty);

  let highlightedGroup =
    switch (CoordsSet.choose(hoveredCells)) {
    | exception Not_found => None
    | coords =>
      switch (CoordsMap.find(coords, groupMap)) {
      | exception Not_found => None
      | group => Some(group)
      }
    };

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

               let onCellHoverStateChange =
                   (change: CellComponent.hoverStateChange) =>
                 switch (change) {
                 | Enter => dispatchHoveredCellChange(Add(coords))
                 | Leave => dispatchHoveredCellChange(Remove(coords))
                 };

               // This is only defined if this cell has a highlighted group.
               let highlightedCellGroup =
                 switch (highlightedGroup) {
                 | Some(group) =>
                   CoordsSet.mem(coords, group.coordsSet)
                     ? Some(group) : None
                 | None => None
                 };

               {
                 state,
                 mined,
                 numAdjacentMines,
                 handleClick,
                 onCellHoverStateChange,
                 isGameOver,
                 coords,
                 recommendedMoveForCell,
                 showOverlay,
                 highlightedCellGroup,
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
                 onCellHoverStateChange,
                 isGameOver,
                 coords,
                 recommendedMoveForCell,
                 showOverlay,
                 highlightedCellGroup,
               }: CellComponent.props,
             ) =>
               <th key={string_of_int(x)}>
                 <CellComponent
                   state
                   mined
                   numAdjacentMines
                   handleClick
                   onCellHoverStateChange
                   isGameOver
                   coords
                   recommendedMoveForCell
                   showOverlay
                   highlightedCellGroup
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
