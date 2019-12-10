open GlobalTypes;

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

  module Visible = {
    let mined = style([backgroundColor(salmon)]);
    let empty = style([backgroundColor(hex("f3f3f3"))]);
  };

  module GameOver = {
    let hidden = merge([hidden, style([hover([backgroundColor(grey)])])]);
  };
};

type click =
  | Right
  | Left;

type handleClick = click => unit;

type props = {
  state: Board.Cell.state,
  mined: bool,
  numAdjacentMines: int,
  handleClick,
  isGameOver: bool,
};

[@react.component]
let make =
    (
      ~state: Board.Cell.state,
      ~mined: bool,
      ~numAdjacentMines: int,
      ~handleClick: handleClick,
      ~isGameOver: bool,
    ) => {
  let (stateClass, inner) =
    switch (state, mined, isGameOver) {
    | (Hidden, _, false) => (Styles.hidden, str(" "))
    | (Hidden, _, true) => (Styles.GameOver.hidden, str(" "))
    | (Flagged, _, _) => (
        Styles.flagged,
        <img className=Styles.flag src="/assets/flag.svg" />,
      )
    | (Visible, true, _) => (
        Styles.Visible.mined,
        <img className=Styles.bomb src="/assets/bomb.svg" />,
      )
    | (Visible, false, _) => (
        Styles.Visible.empty,
        str(string_of_int(numAdjacentMines)),
      )
    };

  let onClick = _ => handleClick(Left);

  let onContextMenu = e => {
    ReactEvent.Mouse.preventDefault(e);
    handleClick(Right);
  };

  let classStyles = Css.merge([Styles.base, stateClass]);

  <section className=classStyles onClick onContextMenu> inner </section>;
};