'use strict';

var Css = require("bs-css/src/Css.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var CustomUtils$ReasonReactExamples = require("../../utils/CustomUtils.bs.js");
var CellComponent$ReasonReactExamples = require("../Cell/CellComponent.bs.js");

var board = Css.style(/* [] */0);

var Style = {
  board: board
};

function getCellProps(param, coords, actionHandler) {
  var handleClick = function (click) {
    var action = click ? /* Check */0 : /* ToggleFlag */1;
    return Curry._2(actionHandler, action, coords);
  };
  return /* record */[
          /* state */param[/* state */0],
          /* mined */param[/* mined */1],
          /* numAdjacentMines */param[/* numAdjacentMines */2],
          /* handleClick */handleClick
        ];
}

function getCellClickHandler(coords, actionHandler, click) {
  var action = click ? /* Check */0 : /* ToggleFlag */1;
  return Curry._2(actionHandler, action, coords);
}

function BoardComponent(Props) {
  var model = Props.model;
  var actionHandler = Props.actionHandler;
  var cellComponents = $$Array.mapi((function (y, modelRow) {
          var cellComponents = $$Array.mapi((function (x, param) {
                  return React.createElement("th", {
                              key: String(x)
                            }, React.createElement(CellComponent$ReasonReactExamples.make, {
                                  state: param[/* state */0],
                                  mined: param[/* mined */1],
                                  numAdjacentMines: param[/* numAdjacentMines */2],
                                  handleClick: param[/* handleClick */3]
                                }));
                }), modelRow);
          return React.createElement("tr", {
                      key: String(y)
                    }, cellComponents);
        }), CustomUtils$ReasonReactExamples.Matrix.map((function (param, coords) {
              var handleClick = function (param) {
                return getCellClickHandler(coords, actionHandler, param);
              };
              return /* record */[
                      /* state */param[/* state */0],
                      /* mined */param[/* mined */1],
                      /* numAdjacentMines */param[/* numAdjacentMines */2],
                      /* handleClick */handleClick
                    ];
            }), model));
  var onContextMenu = function (e) {
    e.preventDefault();
    return /* () */0;
  };
  return React.createElement("table", {
              className: board,
              onContextMenu: onContextMenu
            }, React.createElement("tbody", undefined, cellComponents));
}

var make = BoardComponent;

exports.Style = Style;
exports.getCellProps = getCellProps;
exports.getCellClickHandler = getCellClickHandler;
exports.make = make;
/* board Not a pure module */
