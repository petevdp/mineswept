'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

var ControlPanel = { };

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
          if (action) {
            return /* record */[/* board */Board$ReasonReactExamples.update(/* tuple */[
                          action[0],
                          action[1]
                        ], state[/* board */0])];
          } else {
            return /* record */[/* board */initBoard(/* () */0)];
          }
        }), /* record */[/* board */initBoard(/* () */0)]);
  var dispatch = match[1];
  var boardActionHandler = function (action, coords) {
    return Curry._1(dispatch, /* BoardAction */[
                action,
                coords
              ]);
  };
  var onNewGame = function (param) {
    return Curry._1(dispatch, /* NewGame */0);
  };
  return React.createElement(React.Fragment, {
              children: null
            }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                  onNewGame: onNewGame
                }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                  model: match[0][/* board */0],
                  actionHandler: boardActionHandler
                }));
}

var make = GameComponent;

exports.ControlPanel = ControlPanel;
exports.initBoard = initBoard;
exports.make = make;
/* react Not a pure module */
