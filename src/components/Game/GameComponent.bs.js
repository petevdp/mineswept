'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");

function initBoard(param) {
  return Board$ReasonReactExamples.make(/* tuple */[
              9,
              9
            ], /* :: */[
              /* tuple */[
                0,
                0
              ],
              /* [] */0
            ]);
}

function GameComponent(Props) {
  var match = React.useReducer((function (state, action) {
          if (action.tag) {
            return /* record */[
                    /* board */state[/* board */0],
                    /* boardActionHandler */action[0]
                  ];
          } else {
            return /* record */[
                    /* board */Board$ReasonReactExamples.update(/* tuple */[
                          action[0],
                          action[1]
                        ], state[/* board */0]),
                    /* boardActionHandler */state[/* boardActionHandler */1]
                  ];
          }
        }), /* record */[
        /* board */initBoard(/* () */0),
        /* boardActionHandler */(function (param, param$1) {
            return /* () */0;
          })
      ]);
  var dispatch = match[1];
  var match$1 = match[0];
  React.useEffect((function () {
          Curry._1(dispatch, /* RegisterCellActionHandler */Block.__(1, [(function (action, coords) {
                      return Curry._1(dispatch, /* BoardAction */Block.__(0, [
                                    action,
                                    coords
                                  ]));
                    })]));
          return ;
        }), /* array */[]);
  return React.createElement(BoardComponent$ReasonReactExamples.make, {
              model: match$1[/* board */0],
              actionHandler: match$1[/* boardActionHandler */1]
            });
}

var make = GameComponent;

exports.initBoard = initBoard;
exports.make = make;
/* react Not a pure module */
