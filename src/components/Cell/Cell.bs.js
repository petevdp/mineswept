'use strict';

var Css = require("bs-css/src/Css.js");
var React = require("react");

function str(prim) {
  return prim;
}

function join(list, $$char) {
  if (list) {
    var tail = list[1];
    var tail$1 = list[0];
    if (tail) {
      return tail$1 + ($$char + join(tail, $$char));
    } else {
      return tail$1;
    }
  } else {
    return "";
  }
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

var mine = Css.style(/* :: */[
      Css.backgroundColor(Css.salmon),
      /* [] */0
    ]);

var empty = Css.style(/* :: */[
      Css.backgroundColor(Css.hex("f3f3f3")),
      /* [] */0
    ]);

var Visible = {
  mine: mine,
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
  var model = Props.model;
  var match;
  switch (model[/* state */0]) {
    case /* Hidden */0 :
        match = /* tuple */[
          hidden,
          " "
        ];
        break;
    case /* Visible */1 :
        match = model[/* mined */1] ? /* tuple */[
            mine,
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
        match = /* tuple */[
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
                      match[0],
                      /* [] */0
                    ]
                  ])
            }, match[1]);
}

var make = Cell;

exports.str = str;
exports.join = join;
exports.Styles = Styles;
exports.make = make;
/* cellSize Not a pure module */
