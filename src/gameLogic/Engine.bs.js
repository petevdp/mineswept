'use strict';

var $$Map = require("bs-platform/lib/js/map.js");
var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var CamlinternalOO = require("bs-platform/lib/js/camlinternalOO.js");
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

function compare(a, b) {
  return CustomUtils$ReasonReactExamples.Coords.compare(a[/* coords */0], b[/* coords */0]);
}

function sharedCells(a, b) {
  return Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* affectedCells */2], b[/* affectedCells */2]);
}

function make$1(originCoords, startingMineCount, board) {
  return List.fold_left((function (c, coords) {
                var match = Caml_array.caml_array_get(Caml_array.caml_array_get(board, coords[1]), coords[0]);
                if (typeof match === "number") {
                  if (match !== 0) {
                    return /* record */[
                            /* coords */c[/* coords */0],
                            /* mineCount */c[/* mineCount */1] - 1 | 0,
                            /* affectedCells */c[/* affectedCells */2]
                          ];
                  } else {
                    return /* record */[
                            /* coords */c[/* coords */0],
                            /* mineCount */c[/* mineCount */1],
                            /* affectedCells */Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, coords, c[/* affectedCells */2])
                          ];
                  }
                } else {
                  return c;
                }
              }), /* record */[
              /* coords */originCoords,
              /* mineCount */startingMineCount,
              /* affectedCells */CustomUtils$ReasonReactExamples.CoordsSet.empty
            ], CustomUtils$ReasonReactExamples.Coords.getAdjacent(originCoords, CustomUtils$ReasonReactExamples.Matrix.size(board)));
}

function makeJoin(a, b) {
  return /* record */[
          /* a */a,
          /* b */b,
          /* unionSize */Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* affectedCells */2], b[/* affectedCells */2]))
        ];
}

var Constraint = {
  compare: compare,
  sharedCells: sharedCells,
  make: make$1,
  makeJoin: makeJoin
};

var ConstraintSet = $$Set.Make({
      compare: compare
    });

var ConstraintSetMap = $$Map.Make({
      compare: compare
    });

var ConstraintWithNoCoords = Caml_exceptions.create("Engine-ReasonReactExamples.Constraints.ConstraintWithNoCoords");

function addConstraint(cellConstraintToAdd, map) {
  var map$1 = Curry._3(CustomUtils$ReasonReactExamples.CoordsSetMap.fold, (function (keyCoordsSet, iterConstraints, map) {
          var intersectionCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, keyCoordsSet, cellConstraintToAdd[/* affectedCells */2]);
          if (Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.is_empty, intersectionCoords)) {
            return map;
          } else {
            var intersectionConstraints;
            try {
              intersectionConstraints = Curry._2(CustomUtils$ReasonReactExamples.CoordsSetMap.find, intersectionCoords, map);
            }
            catch (exn){
              if (exn === Caml_builtin_exceptions.not_found) {
                intersectionConstraints = ConstraintSet.empty;
              } else {
                throw exn;
              }
            }
            var intersectionConstraints$1 = Curry._2(ConstraintSet.union, iterConstraints, Curry._2(ConstraintSet.add, cellConstraintToAdd, intersectionConstraints));
            return Curry._3(CustomUtils$ReasonReactExamples.CoordsSetMap.add, intersectionCoords, intersectionConstraints$1, map);
          }
        }), map, CustomUtils$ReasonReactExamples.CoordsSetMap.empty);
  return Curry._3(CustomUtils$ReasonReactExamples.CoordsSetMap.add, cellConstraintToAdd[/* affectedCells */2], Curry._1(ConstraintSet.singleton, cellConstraintToAdd), map$1);
}

function compoundConstraints(set) {
  return Curry._3(ConstraintSet.fold, addConstraint, set, CustomUtils$ReasonReactExamples.CoordsSetMap.empty);
}

var Constraints = {
  ConstraintWithNoCoords: ConstraintWithNoCoords,
  addConstraint: addConstraint,
  compoundConstraints: compoundConstraints
};

var class_tables = [
  0,
  0,
  0
];

function makeMap(constraintSet) {
  return Curry._2(ConstraintSet.fold, (function ($$const, map) {
                if (!class_tables[0]) {
                  var $$class = CamlinternalOO.create_table(0);
                  var env = CamlinternalOO.new_variable($$class, "");
                  var env_init = function (env$1) {
                    var self = CamlinternalOO.create_object_opt(0, $$class);
                    self[env] = env$1;
                    return self;
                  };
                  CamlinternalOO.init_class($$class);
                  class_tables[0] = env_init;
                }
                return Curry._1(class_tables[0], 0);
              }), constraintSet);
}

var Group = {
  makeMap: makeMap
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

function register(r) {
  return Curry._2(CustomUtils$ReasonReactExamples.StrMap.add, r[/* name */0], r);
}

function add(name, description, engine) {
  return register(/* record */[
              /* name */name,
              /* description */description,
              /* engine */engine
            ]);
}

var registry = Curry._1(add("complete", "some more advanced logic that's hard to explain", random), Curry._1(add("random", "randomly picks a square to check", random), CustomUtils$ReasonReactExamples.StrMap.empty));

var Registry = {
  register: register,
  add: add,
  registry: registry
};

console.log("engines available");

Curry._2(CustomUtils$ReasonReactExamples.StrMap.iter, (function (param, v) {
        console.log(v);
        return /* () */0;
      }), registry);

var complete = random;

exports.RestrictedBoard = RestrictedBoard;
exports.Constraint = Constraint;
exports.ConstraintSet = ConstraintSet;
exports.ConstraintSetMap = ConstraintSetMap;
exports.Constraints = Constraints;
exports.Group = Group;
exports.getActionFromEngine = getActionFromEngine;
exports.firstAvailable = firstAvailable;
exports.random = random;
exports.complete = complete;
exports.Registry = Registry;
/* ConstraintSet Not a pure module */
