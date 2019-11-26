'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

function initBoard(param) {
  return Board$ReasonReactExamples.initRandom(/* tuple */[
              5,
              5
            ], 4);
}

function GameComponent(Props) {
  var match = React.useReducer((function (state, action) {
          console.log(/* tuple */[
                "action",
                action
              ]);
          var match = Game$ReasonReactExamples.update(action, state[/* board */0], state[/* gameState */1], initBoard);
          return /* record */[
                  /* board */match[0],
                  /* gameState */match[1]
                ];
        }), /* record */[
        /* board */Board$ReasonReactExamples.initRandom(/* tuple */[
              5,
              5
            ], 4),
        /* gameState : New */0
      ]);
  var dispatch = match[1];
  var match$1 = match[0];
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
                  onNewGame: onNewGame,
                  gameState: match$1[/* gameState */1]
                }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                  model: match$1[/* board */0],
                  handlers: boardHandlers
                }));
}

var make = GameComponent;

exports.initBoard = initBoard;
exports.make = make;
/* react Not a pure module */
