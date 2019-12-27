'use strict';

var List = require("bs-platform/lib/js/list.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");
var Caml_chrome_debugger = require("bs-platform/lib/js/caml_chrome_debugger.js");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var Engine$ReasonReactExamples = require("../../gameLogic/Engine.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

var gameOptions_000 = /* size : tuple */[
  6,
  6
];

var gameOptions_001 = /* minePopulationStrategy */Game$ReasonReactExamples.MinePopulationStrategy.random;

var gameOptions = /* record */Caml_chrome_debugger.record([
    "size",
    "minePopulationStrategy",
    "mineCount"
  ], [
    gameOptions_000,
    gameOptions_001,
    4
  ]);

var initialGameState = Game$ReasonReactExamples.make(gameOptions);

var recommendedMove = Engine$ReasonReactExamples.getActionFromEngine(Engine$ReasonReactExamples.random, initialGameState[/* board */1]);

var startingAppState_000 = /* gameHistory : :: */Caml_chrome_debugger.simpleVariant("::", [
    initialGameState,
    /* [] */0
  ]);

var startingAppState_002 = /* selectedEngineEntry */List.hd(Engine$ReasonReactExamples.registry);

var startingAppState = /* record */Caml_chrome_debugger.record([
    "gameHistory",
    "engineRegistry",
    "selectedEngineEntry",
    "playGameOutWithEngine",
    "fallbackGameInitOptions",
    "recommendedMove"
  ], [
    startingAppState_000,
    Engine$ReasonReactExamples.registry,
    startingAppState_002,
    false,
    gameOptions,
    recommendedMove
  ]);

var NoGameHistory = Caml_exceptions.create("GameComponent-ReasonReactExamples.NoGameHistory");

var NoMovePossible = Caml_exceptions.create("GameComponent-ReasonReactExamples.NoMovePossible");

function GameComponent(Props) {
  var match = React.useReducer((function (prevState, action) {
          var newState;
          if (typeof action === "number") {
            if (action === /* EngineGameAction */0) {
              var recommendedMove = prevState[/* recommendedMove */5];
              var gameHistory = prevState[/* gameHistory */0];
              var gameHistory$1;
              if (recommendedMove !== undefined) {
                if (gameHistory) {
                  gameHistory$1 = Game$ReasonReactExamples.reduce(gameHistory, recommendedMove);
                } else {
                  throw NoGameHistory;
                }
              } else {
                throw NoMovePossible;
              }
              newState = /* record */Caml_chrome_debugger.record([
                  "gameHistory",
                  "engineRegistry",
                  "selectedEngineEntry",
                  "playGameOutWithEngine",
                  "fallbackGameInitOptions",
                  "recommendedMove"
                ], [
                  gameHistory$1,
                  prevState[/* engineRegistry */1],
                  prevState[/* selectedEngineEntry */2],
                  prevState[/* playGameOutWithEngine */3],
                  prevState[/* fallbackGameInitOptions */4],
                  prevState[/* recommendedMove */5]
                ]);
            } else {
              newState = /* record */Caml_chrome_debugger.record([
                  "gameHistory",
                  "engineRegistry",
                  "selectedEngineEntry",
                  "playGameOutWithEngine",
                  "fallbackGameInitOptions",
                  "recommendedMove"
                ], [
                  prevState[/* gameHistory */0],
                  prevState[/* engineRegistry */1],
                  prevState[/* selectedEngineEntry */2],
                  true,
                  prevState[/* fallbackGameInitOptions */4],
                  prevState[/* recommendedMove */5]
                ]);
            }
          } else if (action.tag) {
            var gameHistory$2 = Game$ReasonReactExamples.reduce(prevState[/* gameHistory */0], action[0]);
            newState = /* record */Caml_chrome_debugger.record([
                "gameHistory",
                "engineRegistry",
                "selectedEngineEntry",
                "playGameOutWithEngine",
                "fallbackGameInitOptions",
                "recommendedMove"
              ], [
                gameHistory$2,
                prevState[/* engineRegistry */1],
                prevState[/* selectedEngineEntry */2],
                prevState[/* playGameOutWithEngine */3],
                prevState[/* fallbackGameInitOptions */4],
                prevState[/* recommendedMove */5]
              ]);
          } else {
            var newGame = Game$ReasonReactExamples.make(action[0]);
            newState = /* record */Caml_chrome_debugger.record([
                "gameHistory",
                "engineRegistry",
                "selectedEngineEntry",
                "playGameOutWithEngine",
                "fallbackGameInitOptions",
                "recommendedMove"
              ], [
                Caml_chrome_debugger.simpleVariant("::", [
                    newGame,
                    /* [] */0
                  ]),
                prevState[/* engineRegistry */1],
                prevState[/* selectedEngineEntry */2],
                false,
                prevState[/* fallbackGameInitOptions */4],
                prevState[/* recommendedMove */5]
              ]);
          }
          var match = newState[/* gameHistory */0];
          var newGameState;
          if (match) {
            newGameState = match[0];
          } else {
            throw NoGameHistory;
          }
          var match$1 = newGameState[/* phase */0];
          var recommendedMove$1;
          var exit = 0;
          if (typeof action === "number" && action !== 0) {
            recommendedMove$1 = undefined;
          } else {
            exit = 1;
          }
          if (exit === 1) {
            recommendedMove$1 = typeof match$1 === "number" ? Engine$ReasonReactExamples.getActionFromEngine(newState[/* selectedEngineEntry */2][/* engine */1], newGameState[/* board */1]) : undefined;
          }
          return /* record */Caml_chrome_debugger.record([
                    "gameHistory",
                    "engineRegistry",
                    "selectedEngineEntry",
                    "playGameOutWithEngine",
                    "fallbackGameInitOptions",
                    "recommendedMove"
                  ], [
                    newState[/* gameHistory */0],
                    newState[/* engineRegistry */1],
                    newState[/* selectedEngineEntry */2],
                    newState[/* playGameOutWithEngine */3],
                    newState[/* fallbackGameInitOptions */4],
                    recommendedMove$1
                  ]);
        }), startingAppState);
  var appState = match[0];
  var playGameOutWithEngine = appState[/* playGameOutWithEngine */3];
  var gameHistory = appState[/* gameHistory */0];
  var dispatch = match[1];
  var currGameModel;
  if (gameHistory) {
    currGameModel = gameHistory[0];
  } else {
    throw NoGameHistory;
  }
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
  var boardHandlers = isGameOver ? /* record */Caml_chrome_debugger.record([
        "onCheck",
        "onFlagToggle"
      ], [
        (function (param) {
            return /* () */0;
          }),
        (function (param) {
            return /* () */0;
          })
      ]) : /* record */Caml_chrome_debugger.record([
        "onCheck",
        "onFlagToggle"
      ], [
        (function (coords) {
            return Curry._1(dispatch, /* HumanGameAction */Caml_chrome_debugger.variant("HumanGameAction", 1, [/* Check */Caml_chrome_debugger.variant("Check", 0, [coords])]));
          }),
        (function (coords) {
            return Curry._1(dispatch, /* HumanGameAction */Caml_chrome_debugger.variant("HumanGameAction", 1, [/* ToggleFlag */Caml_chrome_debugger.variant("ToggleFlag", 1, [coords])]));
          })
      ]);
  var panelGameActionHandlers = isGameOver ? /* record */Caml_chrome_debugger.record([
        "onRewindGame",
        "onMakeEngineMove",
        "onPlayGameWithEngine"
      ], [
        (function (param) {
            return /* () */0;
          }),
        (function (param) {
            return /* () */0;
          }),
        (function (param) {
            return /* () */0;
          })
      ]) : /* record */Caml_chrome_debugger.record([
        "onRewindGame",
        "onMakeEngineMove",
        "onPlayGameWithEngine"
      ], [
        (function (steps) {
            return Curry._1(dispatch, /* HumanGameAction */Caml_chrome_debugger.variant("HumanGameAction", 1, [/* Rewind */Caml_chrome_debugger.variant("Rewind", 2, [steps])]));
          }),
        (function (param) {
            return Curry._1(dispatch, /* EngineGameAction */0);
          }),
        (function (param) {
            return Curry._1(dispatch, /* PlayGameWithEngine */1);
          })
      ]);
  var onNewGame = function (param) {
    return Curry._1(dispatch, /* NewGame */Caml_chrome_debugger.variant("NewGame", 0, [gameOptions]));
  };
  var minesLeft = currGameModel[/* mineCount */3] - currGameModel[/* flagCount */2] | 0;
  return React.createElement(React.Fragment, {
              children: null
            }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                  onNewGame: onNewGame,
                  gamePhase: currGameModel[/* phase */0],
                  minesLeft: minesLeft,
                  handlers: panelGameActionHandlers
                }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                  model: currGameModel[/* board */1],
                  handlers: boardHandlers,
                  isGameOver: isGameOver
                }));
}

var gameSize = 6;

var make = GameComponent;

exports.gameSize = gameSize;
exports.gameOptions = gameOptions;
exports.initialGameState = initialGameState;
exports.recommendedMove = recommendedMove;
exports.startingAppState = startingAppState;
exports.NoGameHistory = NoGameHistory;
exports.NoMovePossible = NoMovePossible;
exports.make = make;
/* initialGameState Not a pure module */
