'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var GlobalTypes$ReasonReactExamples = require("../../GlobalTypes.bs.js");

function ControlPanelComponent(Props) {
  var onNewGame = Props.onNewGame;
  var gamePhase = Props.gamePhase;
  var gamePhaseMsg = typeof gamePhase === "number" ? (
      gamePhase !== 0 ? "in progress" : "new"
    ) : (
      gamePhase[0] ? "You lose" : "You Win"
    );
  return React.createElement("section", undefined, React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onNewGame, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("new game")), React.createElement("span", undefined, GlobalTypes$ReasonReactExamples.str(gamePhaseMsg)));
}

var make = ControlPanelComponent;

exports.make = make;
/* react Not a pure module */
