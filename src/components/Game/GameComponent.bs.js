'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
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
  var gameActionDispatch = match[1];
  var gameModel = match[0];
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
  return React.createElement(React.Fragment, {
              children: null
            }, React.createElement(ControlPanelComponent$ReasonReactExamples.make, {
                  onNewGame: onNewGame,
                  gamePhase: gamePhase
                }), React.createElement(BoardComponent$ReasonReactExamples.make, {
                  model: gameModel[/* board */1],
                  handlers: boardHandlers
                }));
}

var make = GameComponent;

exports.make = make;
/* react Not a pure module */
