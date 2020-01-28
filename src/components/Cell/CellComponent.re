open CustomUtils;

module FlagImage = {
  [@bs.module "../../../assets/flag.svg"] external flag: string = "default";
};

module MineImage = {
  [@bs.module "../../../assets/bomb.svg"] external flag: string = "default";
};

Js.log(FlagImage.flag);

module Styles = {
  open Css;
  let cellSize = px(30);

  let base = style([width(cellSize), height(cellSize)]);
  let hidden =
    style([backgroundColor(grey), hover([backgroundColor(darkgrey)])]);
  let flagged =
    style([backgroundColor(green), hover([backgroundColor(lightgreen)])]);

  // Icons
  let iconSize = px(25);
  let flag = style([width(iconSize), height(iconSize)]);
  let bomb =
    style([width(iconSize), height(iconSize), paddingTop(px(2))]);

  let groupHovered = style([backgroundColor(yellow)]);

  module Visible = {
    let mined = style([backgroundColor(salmon)]);
    let empty = style([backgroundColor(hex("f3f3f3"))]);
  };

  module GameOver = {
    let hidden = style([hover([backgroundColor(darkgrey)])]);
  };

  module Recommended = {
    let shouldFlag = style([backgroundColor(red)]);
    let shouldCheck = style([backgroundColor(green)]);
  };
};

type click =
  | Right
  | Left;

type recommendedMoveForCell =
  | Check
  | ToggleFlag;

type handleClick = click => unit;

type hoverStateChange =
  | Leave
  | Enter;

type onCellHoverStateChange = hoverStateChange => unit;

type props = {
  coords: Coords.t,
  state: Board.Cell.state,
  mined: bool,
  numAdjacentMines: int,
  handleClick,
  onCellHoverStateChange,
  isGameOver: bool,
  recommendedMoveForCell: option(recommendedMoveForCell),
  showOverlay: bool,
  /* will be present when this cell's group is highlighted */
  highlightedCellGroup: option(Engine.Group.t),
};

[@react.component]
let make =
    (
      ~coords: Coords.t,
      ~state: Board.Cell.state,
      ~mined: bool,
      ~numAdjacentMines: int,
      ~handleClick: handleClick,
      ~onCellHoverStateChange: onCellHoverStateChange,
      ~isGameOver: bool,
      ~recommendedMoveForCell: option(recommendedMoveForCell),
      ~showOverlay: bool,
      ~highlightedCellGroup: option(Engine.Group.t),
    ) => {
  let isCellGroupHighlighted =
    switch (highlightedCellGroup) {
    | Some(_) => true
    | None => false
    };

  let (stateClass, inner) =
    switch (
      state,
      mined,
      isGameOver,
      isCellGroupHighlighted,
      recommendedMoveForCell,
      showOverlay,
    ) {
    | (Hidden, _, false, true, _, true) => (Styles.groupHovered, str(" "))
    | (Hidden, _, false, _, Some(Check), true) => (
        Styles.Recommended.shouldCheck,
        str(" "),
      )
    | (Hidden, _, false, _, Some(ToggleFlag), true) => (
        Styles.Recommended.shouldFlag,
        str(" "),
      )
    | (Hidden, _, false, _, _, false)
    | (Hidden, _, false, _, None, _) => (Styles.hidden, str(" "))
    | (Hidden, _, true, _, _, _) => (
        Styles.GameOver.hidden,
        numAdjacentMines->string_of_int->str,
      )
    | (Flagged, _, _, _, _, _) => (
        Styles.flagged,
        <img className=Styles.flag src=FlagImage.flag />,
      )
    | (Visible, true, _, _, _, _) => (
        Styles.Visible.mined,
        <img className=Styles.bomb src=MineImage.flag />,
      )
    | (Visible, false, _, _, _, _) => (
        Styles.Visible.empty,
        str(string_of_int(numAdjacentMines)),
      )
    };

  let onClick = _ => handleClick(Left);

  let onContextMenu = e => {
    ReactEvent.Mouse.preventDefault(e);
    handleClick(Right);
  };

  let onMouseEnter = _ => onCellHoverStateChange(Enter);
  let onMouseLeave = _ => onCellHoverStateChange(Leave);

  let classStyles = Css.merge([Styles.base, stateClass]);
  let (x, y) = coords;
  let cell =
    <section
      className=classStyles onClick onContextMenu onMouseLeave onMouseEnter>
      inner
    </section>;
  let groupStatus =
    switch (highlightedCellGroup) {
    | None => ""
    | Some({minMines, maxMines}) => {j|min: $minMines, max: $maxMines|j}
    };

  let cellDataAttributes = [
    ("data-tip", {j|$x, $y, \n $groupStatus |j}),
    ("data-tip-disable", string_of_bool(!showOverlay)),
  ];

  <React.Fragment>
    <DataAttributesProvider data=cellDataAttributes>
      cell
    </DataAttributesProvider>
  </React.Fragment>;
};
