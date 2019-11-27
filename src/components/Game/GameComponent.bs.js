'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Caml_builtin_exceptions = require("bs-platform/lib/js/caml_builtin_exceptions.js");
var Game$ReasonReactExamples = require("../../gameLogic/Game.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");
var ControlPanelComponent$ReasonReactExamples = require("../ControlPanel/ControlPanelComponent.bs.js");

function GameComponent(Props) {
  var match = Game$ReasonReactExamples.useGame(/* record */[
        /* size : tuple */[
          10,
          10
        ],
        /* minePopulationStrategy */Game$ReasonReactExamples.MinePopulationStrategy.random,
        /* mineCount */20
      ]);
  var match$1 = match[0];
  if (match$1) {
    var gameActionDispatch = match[1];
    var gameModel = match$1[0];
    var boardHandlers_000 = function (coords) {
      return Curry._1(gameActionDispatch, /* Check */Block.__(0, [coords]));
    };
    var boardHandlers_001 = function (coords) {
      return Curry._1(gameActionDispatch, /* ToggleFlag */Block.__(1, [coords]));
    };
    var boardHandlers = /* record */[
      boardHandlers_000,
      boardHandlers_001
    ];
    var onNewGame = function (param) {
      return Curry._1(gameActionDispatch, /* NewGame */0);
    };
    var gamePhase = gameModel[/* phase */0];
    var minesLeft = gameModel[/* mineCount */3] - gameModel[/* flagCount */2] | 0;
    return React.createElement(React.Fragment, {
                children: null
              }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                    onNewGame: onNewGame,
                    gamePhase: gamePhase,
                    minesLeft: minesLeft
                  }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                    model: gameModel[/* board */1],
                    handlers: boardHandlers
                  }));
  } else {
    throw [
          Caml_builtin_exceptions.match_failure,
          /* tuple */[
            "GameComponent.re",
            11,
            6
          ]
        ];
  }
}

var make = GameComponent;

exports.make = make;
/* react Not a pure module */
