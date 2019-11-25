'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

function initBoard(param) {
  return Board$ReasonReactExamples.make(/* tuple */[
              10,
              10
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
          return /* record */[/* board */Game$ReasonReactExamples.update(action, state[/* board */0], initBoard)];
        }), /* record */[/* board */initBoard(/* () */0)]);
  var dispatch = match[1];
  var boardHandlers_000 = function (coords) {
    return Curry._1(dispatch, /* Check */Block.__(0, [coords]));
  };
  var boardHandlers_001 = function (coords) {
    return Curry._1(dispatch, /* ToggleFlag */Block.__(1, [coords]));
  };
  var boardHandlers = /* record */[
    boardHandlers_000,
    boardHandlers_001
  ];
  var onNewGame = function (param) {
    return Curry._1(dispatch, /* NewGame */0);
  };
  return React.createElement(React.Fragment, {
              children: null
            }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                  onNewGame: onNewGame
                }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                  model: match[0][/* board */0],
                  handlers: boardHandlers
                }));
}

var make = GameComponent;

exports.initBoard = initBoard;
exports.make = make;
/* react Not a pure module */
