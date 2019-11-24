let str = ReasonReact.string;

type state =
  | Hidden
  | Visible
  | Flagged;

type model = {
  state,
  mined: bool,
  // adjacent: int,
};

let rec join = (list: list(string), ~char) => {
  switch (list) {
  | [] => ""
  | [tail] => tail
  | [head, ...tail] => head ++ char ++ join(tail, ~char)
  };
};
module Styles = {
  open Css;
  let cellSize = px(30);

  let base = style([width(cellSize), height(cellSize)]);
  let hidden = style([backgroundColor(grey)]);
  let flagged = style([backgroundColor(green)]);

  // Icons
  let iconSize = px(25);
  let flag = style([width(iconSize), height(iconSize)]);
  let bomb =
    style([width(iconSize), height(iconSize), paddingTop(px(2))]);

  module Visible = {
    let mine = style([backgroundColor(salmon)]);
    let empty = style([backgroundColor(hex("f3f3f3"))]);
  };
};

[@react.component]
let make = (~model: model) => {
  let {state, mined} = model;
  let (stateClass, inner) =
    switch (state, mined) {
    | (Hidden, _) => (Styles.hidden, str(" "))
    | (Flagged, _) => (
        Styles.flagged,
        <img className=Styles.flag src="/assets/flag.svg" />,
      )
    | (Visible, true) => (
        Styles.Visible.mine,
        <img className=Styles.bomb src="/assets/bomb.svg" />,
      )
    | (Visible, false) => (Styles.Visible.empty, str(" "))
    // | Visible, false => Styles.Visible.empty
    };
  <div className={Css.merge([Styles.base, stateClass])}> inner </div>;
};