'use strict';

var $$Array = require("bs-platform/lib/js/array.js");
var React = require("react");
var Board$ReasonReactExamples = require("../../gameLogic/Board.bs.js");
var CellComponent$ReasonReactExamples = require("../Cell/CellComponent.bs.js");

function BoardComponent(Props) {
  var cellModelMatrix = Props.cellModelMatrix;
  var cellComponents = $$Array.map((function (row) {
          var cellComponents = $$Array.map((function (param) {
                  return React.createElement(CellComponent$ReasonReactExamples.make, {
                              state: param[/* state */0],
                              mined: param[/* mined */1],
                              handleClick: param[/* handleClick */3]
                            });
                }), row);
          return React.createElement("tr", undefined, cellComponents);
        }), cellModelMatrix);
  return React.createElement("table", {
              className: "board"
            }, React.createElement("tbody", undefined, cellComponents));
}

var adjacentDiff = Board$ReasonReactExamples.adjacentDiff;

var make = BoardComponent;

exports.adjacentDiff = adjacentDiff;
exports.make = make;
/* react Not a pure module */
