'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var GlobalTypes$ReasonReactExamples = require("../../GlobalTypes.bs.js");

function ControlPanelComponent(Props) {
  var onNewGame = Props.onNewGame;
  return React.createElement("section", undefined, React.createElement("button", {
                  onClick: (function (param) {
                      return Curry._1(onNewGame, /* () */0);
                    })
                }, GlobalTypes$ReasonReactExamples.str("new game")));
}

var make = ControlPanelComponent;

exports.make = make;
/* react Not a pure module */
