'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var CustomUtils$ReasonReactExamples = require("../../utils/CustomUtils.bs.js");
var BoardComponent$ReasonReactExamples = require("../Board/BoardComponent.bs.js");

function getCellProps(param, coords, actionHandler) {
  var handleClick = function (click) {
    var action = click ? /* Check */0 : /* ToggleFlag */1;
    return Curry._2(actionHandler, coords, action);
  };
  return /* record */[
          /* state */param[/* state */0],
          /* mined */param[/* mined */1],
          /* numAdjacentMines */param[/* numAdjacentMines */2],
          /* handleClick */handleClick
        ];
}

function GameComponent(Props) {
  var actionHandler = function (coords, action) {
    console.log("action: ");
    console.log(coords);
    console.log(action);
    return /* () */0;
  };
  var board = CustomUtils$ReasonReactExamples.Matrix.map((function (model, coords) {
          return getCellProps(model, coords, actionHandler);
        }), Board$ReasonReactExamples.make(/* tuple */[
            10,
            10
          ], /* :: */[
            /* tuple */[
              1,
              1
            ],
            /* [] */0
          ]));
  return React.createElement(BoardComponent$ReasonReactExamples.make, {
              cellModelMatrix: board,
              actionHandler: actionHandler
            });
}

var make = GameComponent;

exports.getCellProps = getCellProps;
exports.make = make;
/* react Not a pure module */
