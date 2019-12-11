'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var GlobalTypes$ReasonReactExamples = require("../../GlobalTypes.bs.js");

function ControlPanelComponent(Props) {
  var onNewGame = Props.onNewGame;
  var gamePhase = Props.gamePhase;
  var minesLeft = Props.minesLeft;
  var handlers = Props.handlers;
  var gamePhaseMsg = typeof gamePhase === "number" ? (
      gamePhase !== 0 ? "in progress" : "new"
    ) : (
      gamePhase[0] ? "You lose" : "You Win"
    );
  var onPlayGameWithEngine = handlers[/* onPlayGameWithEngine */2];
  var onMakeEngineMove = handlers[/* onMakeEngineMove */1];
  var onRewindGame = handlers[/* onRewindGame */0];
  return React.createElement("section", undefined, React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onNewGame, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("new game")), React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onRewindGame, 1);
                    })
                }, GlobalTypes$ReasonReactExamples.str("rewind game")), React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onMakeEngineMove, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("Make Engine Move")), React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onPlayGameWithEngine, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("Play game out with engine")), React.createElement("span", undefined, GlobalTypes$ReasonReactExamples.str(gamePhaseMsg)), React.createElement("span", undefined, GlobalTypes$ReasonReactExamples.str("  mines left: " + String(minesLeft))));
}

var make = ControlPanelComponent;

exports.make = make;
/* react Not a pure module */
