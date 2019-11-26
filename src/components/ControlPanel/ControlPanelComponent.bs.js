'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var GlobalTypes$ReasonReactExamples = require("../../GlobalTypes.bs.js");

function ControlPanelComponent(Props) {
  var onNewGame = Props.onNewGame;
  var gameState = Props.gameState;
  var gameStateMsg;
  switch (gameState) {
    case /* New */0 :
        gameStateMsg = "new";
        break;
    case /* Playing */1 :
        gameStateMsg = "in progress";
        break;
    case /* Ended */2 :
        gameStateMsg = "game over";
        break;
    
  }
  return React.createElement("section", undefined, React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onNewGame, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("new game")), React.createElement("span", undefined, GlobalTypes$ReasonReactExamples.str(gameStateMsg)));
}

var make = ControlPanelComponent;

exports.make = make;
/* react Not a pure module */
