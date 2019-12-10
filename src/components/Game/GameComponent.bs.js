'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Caml_builtin_exceptions = require("bs-platform/lib/js/caml_builtin_exceptions.js");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var Engine$ReasonReactExamples = require("../../gameLogic/Engine.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

var gameOptions_000 = /* size : tuple */[
  4,
  4
];

var gameOptions_001 = /* minePopulationStrategy */Game$ReasonReactExamples.MinePopulationStrategy.random;

var gameOptions = /* record */[
  gameOptions_000,
  gameOptions_001,
  /* mineCount */2
];

function GameComponent(Props) {
  var match = React.useReducer((function (prevState, action) {
          if (typeof action === "number") {
            return /* record */[
                    /* gameHistory */prevState[/* gameHistory */0],
                    /* selectedEngine */prevState[/* selectedEngine */1],
                    /* playGameOutWithEngine */true,
                    /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                  ];
          } else {
            switch (action.tag | 0) {
              case /* NewGame */0 :
                  return /* record */[
                          /* gameHistory : :: */[
                            Game$ReasonReactExamples.make(action[0]),
                            /* [] */0
                          ],
                          /* selectedEngine */prevState[/* selectedEngine */1],
                          /* playGameOutWithEngine */prevState[/* playGameOutWithEngine */2],
                          /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                        ];
              case /* HumanGameAction */1 :
                  return /* record */[
                          /* gameHistory */Game$ReasonReactExamples.reduce(prevState[/* gameHistory */0], action[0]),
                          /* selectedEngine */prevState[/* selectedEngine */1],
                          /* playGameOutWithEngine */prevState[/* playGameOutWithEngine */2],
                          /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                        ];
              case /* EngineGameAction */2 :
                  var selectedEngine = prevState[/* selectedEngine */1];
                  var gameHistory = prevState[/* gameHistory */0];
                  var gameHistory$1;
                  if (gameHistory) {
                    gameHistory$1 = Game$ReasonReactExamples.reduce(gameHistory, Engine$ReasonReactExamples.getActionFromEngine(selectedEngine, gameHistory[0][/* board */1]));
                  } else {
                    var gameState = Game$ReasonReactExamples.make(prevState[/* fallbackGameInitOptions */3]);
                    gameHistory$1 = Game$ReasonReactExamples.reduce(/* :: */[
                          gameState,
                          /* [] */0
                        ], Engine$ReasonReactExamples.getActionFromEngine(selectedEngine, gameState[/* board */1]));
                  }
                  return /* record */[
                          /* gameHistory */gameHistory$1,
                          /* selectedEngine */prevState[/* selectedEngine */1],
                          /* playGameOutWithEngine */prevState[/* playGameOutWithEngine */2],
                          /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                        ];
              
            }
          }
        }), /* record */[
        /* gameHistory : :: */[
          Game$ReasonReactExamples.make(gameOptions),
          /* [] */0
        ],
        /* selectedEngine */Engine$ReasonReactExamples.firstAvailable,
        /* playGameOutWithEngine */false,
        /* fallbackGameInitOptions */gameOptions
      ]);
  var dispatch = match[1];
  var boardHandlers_000 = function (coords) {
    return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* Check */Block.__(0, [coords])]));
  };
  var boardHandlers_001 = function (coords) {
    return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* ToggleFlag */Block.__(1, [coords])]));
  };
  var boardHandlers = /* record */[
    boardHandlers_000,
    boardHandlers_001
  ];
  var gameHistory = match[0][/* gameHistory */0];
  if (gameHistory) {
    var currGameModel = gameHistory[0];
    var onNewGame = function (param) {
      return Curry._1(dispatch, /* NewGame */Block.__(0, [gameOptions]));
    };
    var onRewindGame = function (steps) {
      return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* Rewind */Block.__(2, [steps])]));
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
            72,
            6
          ]
        ];
  }
}

var make = GameComponent;

exports.gameOptions = gameOptions;
exports.make = make;
/* react Not a pure module */
