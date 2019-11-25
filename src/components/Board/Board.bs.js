'use strict';

var $$Array = require("bs-platform/lib/js/array.js");
var React = require("react");
var Cell$ReasonReactExamples = require("../Cell/Cell.bs.js");

function Board(Props) {
  var m = Props.cellModelMatrix;
  var rows = $$Array.map((function (row) {
          var cells = $$Array.map((function (model) {
                  return React.createElement("td", undefined, React.createElement(Cell$ReasonReactExamples.make, {
                                  model: model
                                }));
                }), row);
          return React.createElement("tr", undefined, cells);
        }), m);
  return React.createElement("table", {
              className: "board"
            }, React.createElement("tbody", undefined, rows));
}

var make = Board;

exports.make = make;
/* react Not a pure module */
