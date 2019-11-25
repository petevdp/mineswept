'use strict';

var Css = require("bs-css/src/Css.js");
var React = require("react");

function str(prim) {
  return prim;
}

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

function Cell(Props) {
  var match = Props.model;
  var match$1;
  switch (match[/* state */0]) {
    case /* Hidden */0 :
        match$1 = /* tuple */[
          hidden,
          " "
        ];
        break;
    case /* Visible */1 :
        match$1 = match[/* mined */1] ? /* tuple */[
            mined,
            React.createElement("img", {
                  className: bomb,
                  src: "/assets/bomb.svg"
                })
          ] : /* tuple */[
            empty,
            " "
          ];
        break;
    case /* Flagged */2 :
        match$1 = /* tuple */[
          flagged,
          React.createElement("img", {
                className: flag,
                src: "/assets/flag.svg"
              })
        ];
        break;
    
  }
  return React.createElement("div", {
              className: Css.merge(/* :: */[
                    base,
                    /* :: */[
                      match$1[0],
                      /* [] */0
                    ]
                  ]),
              onClick: match[/* onClick */2]
            }, match$1[1]);
}

var make = Cell;

exports.str = str;
exports.Styles = Styles;
exports.make = make;
/* cellSize Not a pure module */
