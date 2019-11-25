'use strict';

var Css = require("bs-css/src/Css.js");
var $$Array = require("bs-platform/lib/js/array.js");
var React = require("react");
var CellComponent$ReasonReactExamples = require("../Cell/CellComponent.bs.js");

var board = Css.style(/* [] */0);

var Style = {
  board: board
};

function BoardComponent(Props) {
  var cellModelMatrix = Props.cellModelMatrix;
  Props.actionHandler;
  var cellComponents = $$Array.mapi((function (i, modelRow) {
          var cellComponents = $$Array.mapi((function (j, param) {
                  return React.createElement("th", {
                              key: String(j)
                            }, React.createElement(CellComponent$ReasonReactExamples.make, {
                                  state: param[/* state */0],
                                  mined: param[/* mined */1],
                                  numAdjacentMines: param[/* numAdjacentMines */2],
                                  handleClick: param[/* handleClick */3]
                                }));
                }), modelRow);
          return React.createElement("tr", {
                      key: String(i)
                    }, cellComponents);
        }), cellModelMatrix);
  return React.createElement("table", {
              className: board
            }, React.createElement("tbody", undefined, cellComponents));
}

var make = BoardComponent;

exports.Style = Style;
exports.make = make;
/* board Not a pure module */
