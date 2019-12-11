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
  6,
  6
];

var gameOptions_001 = /* minePopulationStrategy */Game$ReasonReactExamples.MinePopulationStrategy.random;

var gameOptions = /* record */[
  gameOptions_000,
  gameOptions_001,
  /* mineCount */4
];

var startingAppState_000 = /* gameHistory : :: */[
  Game$ReasonReactExamples.make(gameOptions),
  /* [] */0
];

var startingAppState = /* record */[
  startingAppState_000,
  /* selectedEngine */Engine$ReasonReactExamples.solver1,
  /* playGameOutWithEngine */false,
  /* fallbackGameInitOptions */gameOptions
];

function GameComponent(Props) {
  var match = React.useReducer((function (prevState, action) {
          if (typeof action === "number") {
            if (action === /* EngineGameAction */0) {
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
            } else {
              return /* record */[
                      /* gameHistory */prevState[/* gameHistory */0],
                      /* selectedEngine */prevState[/* selectedEngine */1],
                      /* playGameOutWithEngine */true,
                      /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                    ];
            }
          } else if (action.tag) {
            return /* record */[
                    /* gameHistory */Game$ReasonReactExamples.reduce(prevState[/* gameHistory */0], action[0]),
                    /* selectedEngine */prevState[/* selectedEngine */1],
                    /* playGameOutWithEngine */prevState[/* playGameOutWithEngine */2],
                    /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                  ];
          } else {
            return /* record */[
                    /* gameHistory : :: */[
                      Game$ReasonReactExamples.make(action[0]),
                      /* [] */0
                    ],
                    /* selectedEngine */prevState[/* selectedEngine */1],
                    /* playGameOutWithEngine */false,
                    /* fallbackGameInitOptions */prevState[/* fallbackGameInitOptions */3]
                  ];
          }
        }), startingAppState);
  var appState = match[0];
  var playGameOutWithEngine = appState[/* playGameOutWithEngine */2];
  var gameHistory = appState[/* gameHistory */0];
  var dispatch = match[1];
  if (gameHistory) {
    var currGameModel = gameHistory[0];
    var match$1 = currGameModel[/* phase */0];
    var isGameOver = typeof match$1 === "number" ? false : true;
    React.useEffect((function (param) {
            if (playGameOutWithEngine && !isGameOver) {
              Curry._1(dispatch, /* EngineGameAction */0);
            }
            return (function (param) {
                      return /* () */0;
                    });
          }), /* tuple */[
          gameHistory,
          playGameOutWithEngine,
          currGameModel
        ]);
    if (gameHistory) {
      var currGameModel$1 = gameHistory[0];
      var match$2 = currGameModel$1[/* phase */0];
      var isGameOver$1 = typeof match$2 === "number" ? false : true;
      var boardHandlers = isGameOver$1 ? /* record */[
          /* onCheck */(function (param) {
              return /* () */0;
            }),
          /* onFlagToggle */(function (param) {
              return /* () */0;
            })
        ] : /* record */[
          /* onCheck */(function (coords) {
              return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* Check */Block.__(0, [coords])]));
            }),
          /* onFlagToggle */(function (coords) {
              return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* ToggleFlag */Block.__(1, [coords])]));
            })
        ];
      var panelGameActionHandlers = isGameOver$1 ? /* record */[
          /* onRewindGame */(function (param) {
              return /* () */0;
            }),
          /* onMakeEngineMove */(function (param) {
              return /* () */0;
            }),
          /* onPlayGameWithEngine */(function (param) {
              return /* () */0;
            })
        ] : /* record */[
          /* onRewindGame */(function (steps) {
              return Curry._1(dispatch, /* HumanGameAction */Block.__(1, [/* Rewind */Block.__(2, [steps])]));
            }),
          /* onMakeEngineMove */(function (param) {
              return Curry._1(dispatch, /* EngineGameAction */0);
            }),
          /* onPlayGameWithEngine */(function (param) {
              return Curry._1(dispatch, /* PlayGameWithEngine */1);
            })
        ];
      var onNewGame = function (param) {
        return Curry._1(dispatch, /* NewGame */Block.__(0, [gameOptions]));
      };
      var minesLeft = currGameModel$1[/* mineCount */3] - currGameModel$1[/* flagCount */2] | 0;
      return React.createElement(React.Fragment, {
                  children: null
                }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                      onNewGame: onNewGame,
                      gamePhase: currGameModel$1[/* phase */0],
                      minesLeft: minesLeft,
                      handlers: panelGameActionHandlers
                    }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                      model: currGameModel$1[/* board */1],
                      handlers: boardHandlers,
                      isGameOver: isGameOver$1
                    }));
    } else {
      throw [
            Caml_builtin_exceptions.match_failure,
            /* tuple */[
              "GameComponent.re",
              85,
              6
            ]
          ];
    }
  } else {
    throw [
          Caml_builtin_exceptions.match_failure,
          /* tuple */[
            "GameComponent.re",
            67,
            6
          ]
        ];
  }
}

var gameSize = 6;

var make = GameComponent;

exports.gameSize = gameSize;
exports.gameOptions = gameOptions;
exports.startingAppState = startingAppState;
exports.make = make;
/* startingAppState Not a pure module */
