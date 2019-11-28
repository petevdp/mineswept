'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Caml_builtin_exceptions = require("bs-platform/lib/js/caml_builtin_exceptions.js");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

var gameOptions_000 = /* size : tuple */[
  10,
  10
];

var gameOptions_001 = /* minePopulationStrategy */Game$ReasonReactExamples.MinePopulationStrategy.random;

var gameOptions = /* record */[
  gameOptions_000,
  gameOptions_001,
  /* mineCount */25
];

function GameComponent(Props) {
  var match = React.useReducer((function (prevState, action) {
          if (action.tag) {
            return /* record */[/* gameHistory : :: */[
                      Game$ReasonReactExamples.make(action[0]),
                      /* [] */0
                    ]];
          } else {
            return /* record */[/* gameHistory */Game$ReasonReactExamples.reduce(prevState[/* gameHistory */0], action[0])];
          }
        }), /* record */[/* gameHistory : :: */[
          Game$ReasonReactExamples.make(gameOptions),
          /* [] */0
        ]]);
  var dispatch = match[1];
  var boardHandlers_000 = function (coords) {
    return Curry._1(dispatch, /* GameAction */Block.__(0, [/* Check */Block.__(0, [coords])]));
  };
  var boardHandlers_001 = function (coords) {
    return Curry._1(dispatch, /* GameAction */Block.__(0, [/* ToggleFlag */Block.__(1, [coords])]));
  };
  var boardHandlers = /* record */[
    boardHandlers_000,
    boardHandlers_001
  ];
  var gameHistory = match[0][/* gameHistory */0];
  if (gameHistory) {
    var currGameModel = gameHistory[0];
    var onNewGame = function (param) {
      return Curry._1(dispatch, /* NewGame */Block.__(1, [gameOptions]));
    };
    var onRewindGame = function (steps) {
      return Curry._1(dispatch, /* GameAction */Block.__(0, [/* Rewind */Block.__(2, [steps])]));
    };
    var minesLeft = currGameModel[/* mineCount */3] - currGameModel[/* flagCount */2] | 0;
    return React.createElement(React.Fragment, {
                children: null
              }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                    onNewGame: onNewGame,
                    gamePhase: currGameModel[/* phase */0],
                    minesLeft: minesLeft,
                    onRewindGame: onRewindGame
                  }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                    model: currGameModel[/* board */1],
                    handlers: boardHandlers
                  }));
  } else {
    throw [
          Caml_builtin_exceptions.match_failure,
          /* tuple */[
            "GameComponent.re",
            38,
            6
          ]
        ];
  }
}

var make = GameComponent;

exports.gameOptions = gameOptions;
exports.make = make;
/* react Not a pure module */
