'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");
var Caml_builtin_exceptions = require("bs-platform/lib/js/caml_builtin_exceptions.js");
var Board$ReasonReactExamples = require("./Board.bs.js");
var CustomUtils$ReasonReactExamples = require("../utils/CustomUtils.bs.js");

var InvalidBoard = Caml_exceptions.create("Engine-ReasonReactExamples.RestrictedBoard.InvalidBoard");

function make(board) {
  if (Board$ReasonReactExamples.hasVisibleMines(board)) {
    throw [
          InvalidBoard,
          "the board has visible mines!"
        ];
  }
  return CustomUtils$ReasonReactExamples.Matrix.map((function (param, param$1) {
                switch (param[/* state */0]) {
                  case /* Hidden */0 :
                      return /* Hidden */0;
                  case /* Visible */1 :
                      return /* Visible */[param[/* numAdjacentMines */2]];
                  case /* Flagged */2 :
                      return /* Flagged */1;
                  
                }
              }), board);
}

var RestrictedBoard = {
  InvalidBoard: InvalidBoard,
  make: make
};

function getActionFromEngine(engine, board) {
  return Curry._1(engine, make(board));
}

function firstAvailable(board) {
  var match = List.hd(List.filter((function (param) {
                return param[0] === /* Hidden */0;
              }))($$Array.to_list(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board))));
  return /* Check */Block.__(0, [match[1]]);
}

function random(board) {
  var match = List.hd(Belt_List.shuffle(List.filter((function (param) {
                    return param[0] === /* Hidden */0;
                  }))($$Array.to_list(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board)))));
  return /* Check */Block.__(0, [match[1]]);
}

function allHidden(list) {
  return !Belt_List.some(list, (function (cell) {
                return cell !== /* Hidden */0;
              }));
}

var Helpers = {
  allHidden: allHidden
};

function make$1(coords, boardSize, numMines, board) {
  return List.fold_left((function (param, param$1) {
                var y = param$1[1];
                var x = param$1[0];
                var coordsSet = param[/* coordsSet */1];
                var numMines = param[/* numMines */0];
                var match = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
                if (typeof match === "number") {
                  if (match !== 0) {
                    return /* record */[
                            /* numMines */numMines - 1 | 0,
                            /* coordsSet */coordsSet
                          ];
                  } else {
                    return /* record */[
                            /* numMines */numMines,
                            /* coordsSet */Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, /* tuple */[
                                  x,
                                  y
                                ], coordsSet)
                          ];
                  }
                } else {
                  return /* record */[
                          /* numMines */numMines,
                          /* coordsSet */coordsSet
                        ];
                }
              }), /* record */[
              /* numMines */numMines,
              /* coordsSet */CustomUtils$ReasonReactExamples.CoordsSet.empty
            ], CustomUtils$ReasonReactExamples.Coords.getAdjacent(coords, boardSize));
}

function getNumberGroups(board) {
  var cellList = $$Array.to_list(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board));
  var size = CustomUtils$ReasonReactExamples.Matrix.size(board);
  return List.fold_left((function (map, param) {
                var coords = param[1];
                var cell = param[0];
                if (typeof cell === "number") {
                  return map;
                } else {
                  var mineCount = cell[0];
                  if (mineCount === 0) {
                    return map;
                  } else {
                    var group = make$1(coords, size, mineCount, board);
                    var val;
                    try {
                      val = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, group[/* coordsSet */1]);
                    }
                    catch (exn){
                      if (exn === Caml_builtin_exceptions.not_found) {
                        return map;
                      } else {
                        throw exn;
                      }
                    }
                    return Curry._3(CustomUtils$ReasonReactExamples.CoordsMap.add, coords, group, map);
                  }
                }
              }), CustomUtils$ReasonReactExamples.CoordsMap.empty, cellList);
}

function isFlaggable(param, param$1) {
  return List.length(Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.elements, param$1[/* coordsSet */1])) === param$1[/* numMines */0];
}

function noMines(param, param$1) {
  return param$1[/* numMines */0] === 0;
}

var BaseGroup = {
  make: make$1,
  getNumberGroups: getNumberGroups,
  isFlaggable: isFlaggable,
  noMines: noMines
};

function solver1(board) {
  var groups = getNumberGroups(board);
  var val;
  try {
    val = Curry._1(CustomUtils$ReasonReactExamples.CoordsMap.choose, Curry._2(CustomUtils$ReasonReactExamples.CoordsMap.filter, isFlaggable, groups));
  }
  catch (exn){
    if (exn === Caml_builtin_exceptions.not_found) {
      var exit = 0;
      var val$1;
      try {
        val$1 = Curry._1(CustomUtils$ReasonReactExamples.CoordsMap.choose, Curry._2(CustomUtils$ReasonReactExamples.CoordsMap.filter, noMines, groups));
        exit = 2;
      }
      catch (exn$1){
        if (exn$1 === Caml_builtin_exceptions.not_found) {
          console.log("random choice");
          return random(board);
        } else {
          throw exn$1;
        }
      }
      if (exit === 2) {
        console.log("found empty cell, checking...");
        return /* Check */Block.__(0, [Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, val$1[1][/* coordsSet */1])]);
      }
      
    } else {
      throw exn;
    }
  }
  console.log("found flaggable cell, flagging...");
  return /* ToggleFlag */Block.__(1, [Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, val[1][/* coordsSet */1])]);
}

exports.RestrictedBoard = RestrictedBoard;
exports.getActionFromEngine = getActionFromEngine;
exports.firstAvailable = firstAvailable;
exports.random = random;
exports.Helpers = Helpers;
exports.BaseGroup = BaseGroup;
exports.solver1 = solver1;
/* Board-ReasonReactExamples Not a pure module */
