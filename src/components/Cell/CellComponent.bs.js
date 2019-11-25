'use strict';

var Css = require("bs-css/src/Css.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var GlobalTypes$ReasonReactExamples = require("../../GlobalTypes.bs.js");

var cellSize = Css.px(30);

var base = Css.style(/* :: */[
      Css.width(cellSize),
      /* :: */[
        Css.height(cellSize),
        /* [] */0
      ]
    ]);

var hidden = Css.style(/* :: */[
      Css.backgroundColor(Css.grey),
      /* [] */0
    ]);

var flagged = Css.style(/* :: */[
      Css.backgroundColor(Css.green),
      /* [] */0
    ]);

var iconSize = Css.px(25);

var flag = Css.style(/* :: */[
      Css.width(iconSize),
      /* :: */[
        Css.height(iconSize),
        /* [] */0
      ]
    ]);

var bomb = Css.style(/* :: */[
      Css.width(iconSize),
      /* :: */[
        Css.height(iconSize),
        /* :: */[
          Css.paddingTop(Css.px(2)),
          /* [] */0
        ]
      ]
    ]);

var mined = Css.style(/* :: */[
      Css.backgroundColor(Css.salmon),
      /* [] */0
    ]);

var empty = Css.style(/* :: */[
      Css.backgroundColor(Css.hex("f3f3f3")),
      /* [] */0
    ]);

var Visible = {
  mined: mined,
  empty: empty
};

var Styles = {
  cellSize: cellSize,
  base: base,
  hidden: hidden,
  flagged: flagged,
  iconSize: iconSize,
  flag: flag,
  bomb: bomb,
  Visible: Visible
};

function CellComponent(Props) {
  var state = Props.state;
  var mined$1 = Props.mined;
  var numAdjacentMines = Props.numAdjacentMines;
  var handleClick = Props.handleClick;
  var match;
  switch (state) {
    case /* Hidden */0 :
        match = /* tuple */[
          hidden,
          GlobalTypes$ReasonReactExamples.str(" ")
        ];
        break;
    case /* Visible */1 :
        match = mined$1 ? /* tuple */[
            mined,
            React.createElement("img", {
                  className: bomb,
                  src: "/assets/bomb.svg"
                })
          ] : /* tuple */[
            empty,
            GlobalTypes$ReasonReactExamples.str(String(numAdjacentMines))
          ];
        break;
    case /* Flagged */2 :
        match = /* tuple */[
          flagged,
          React.createElement("img", {
                className: flag,
                src: "/assets/flag.svg"
              })
        ];
        break;
    
  }
  var onClick = function (param) {
    return Curry._1(handleClick, /* Left */1);
  };
  var onContextMenu = function (e) {
    e.preventDefault();
    return Curry._1(handleClick, /* Right */0);
  };
  return React.createElement("div", {
              className: Css.merge(/* :: */[
                    base,
                    /* :: */[
                      match[0],
                      /* [] */0
                    ]
                  ]),
              onClick: onClick,
              onContextMenu: onContextMenu
            }, match[1]);
}

var make = CellComponent;

exports.Styles = Styles;
exports.make = make;
/* cellSize Not a pure module */
