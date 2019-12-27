'use strict';

var Css = require("bs-css/src/Css.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");
var Caml_chrome_debugger = require("bs-platform/lib/js/caml_chrome_debugger.js");
var CustomUtils$ReasonReactExamples = require("../../utils/CustomUtils.bs.js");
var CellComponent$ReasonReactExamples = require("../Cell/CellComponent.bs.js");

var board = Css.style(/* [] */0);

var Style = {
  board: board
};

function BoardComponent(Props) {
  var model = Props.model;
  var handlers = Props.handlers;
  var isGameOver = Props.isGameOver;
  var cellComponents = $$Array.mapi((function (y, modelRow) {
          var cellComponents = $$Array.mapi((function (x, param) {
                  return React.createElement("th", {
                              key: String(x)
                            }, React.createElement(CellComponent$ReasonReactExamples.make, {
                                  state: param[/* state */0],
                                  mined: param[/* mined */1],
                                  numAdjacentMines: param[/* numAdjacentMines */2],
                                  handleClick: param[/* handleClick */3],
                                  isGameOver: param[/* isGameOver */4]
                                }));
                }), modelRow);
          return React.createElement("tr", {
                      key: String(y)
                    }, cellComponents);
        }), CustomUtils$ReasonReactExamples.Matrix.map((function (param, coords) {
              var handleClick = function (click) {
                if (click) {
                  Curry._1(handlers[/* onCheck */0], coords);
                } else {
                  Curry._1(handlers[/* onFlagToggle */1], coords);
                }
                return /* () */0;
              };
              return /* record */Caml_chrome_debugger.record([
                        "state",
                        "mined",
                        "numAdjacentMines",
                        "handleClick",
                        "isGameOver"
                      ], [
                        param[/* state */0],
                        param[/* mined */1],
                        param[/* numAdjacentMines */2],
                        handleClick,
                        isGameOver
                      ]);
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
exports.make = make;
/* board Not a pure module */
