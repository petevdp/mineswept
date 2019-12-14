'use strict';

var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");
var Caml_builtin_exceptions = require("bs-platform/lib/js/caml_builtin_exceptions.js");
var Engine$ReasonReactExamples = require("./Engine.bs.js");
var CustomUtils$ReasonReactExamples = require("../utils/CustomUtils.bs.js");

function compare(a, b) {
  return CustomUtils$ReasonReactExamples.Coords.compare(a[/* coords */0], b[/* coords */0]);
}

function sharedCells(a, b) {
  return Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* effectedCells */2], b[/* effectedCells */2]);
}

function make(originCoords, startingMineCount, board) {
  return List.fold_left((function (c, coords) {
                var match = Caml_array.caml_array_get(Caml_array.caml_array_get(board, coords[1]), coords[0]);
                if (typeof match === "number") {
                  if (match !== 0) {
                    return /* record */[
                            /* coords */c[/* coords */0],
                            /* mineCount */c[/* mineCount */1] - 1 | 0,
                            /* effectedCells */c[/* effectedCells */2]
                          ];
                  } else {
                    return /* record */[
                            /* coords */c[/* coords */0],
                            /* mineCount */c[/* mineCount */1],
                            /* effectedCells */Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, coords, c[/* effectedCells */2])
                          ];
                  }
                } else {
                  return c;
                }
              }), /* record */[
              /* coords */originCoords,
              /* mineCount */startingMineCount,
              /* effectedCells */CustomUtils$ReasonReactExamples.CoordsSet.empty
            ], CustomUtils$ReasonReactExamples.Coords.getAdjacent(originCoords, CustomUtils$ReasonReactExamples.Matrix.size(board)));
}

var Constraint = {
  compare: compare,
  sharedCells: sharedCells,
  make: make
};

var ConstraintSet = $$Set.Make({
      compare: compare
    });

var ConstraintWithNoCoords = Caml_exceptions.create("AltEngine-ReasonReactExamples.Constraints.ConstraintWithNoCoords");

function addConstraint(cellConstraintToAdd, map) {
  var map$1 = Curry._3(Engine$ReasonReactExamples.CoordsSetMap.fold, (function (keyCoordsSet, cellConstraintSet, map) {
          var intersectingCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, keyCoordsSet, cellConstraintToAdd[/* effectedCells */2]);
          if (Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.is_empty, intersectingCoords)) {
            return map;
          } else {
            var existingConstraints;
            try {
              existingConstraints = Curry._2(Engine$ReasonReactExamples.CoordsSetMap.find, intersectingCoords, map);
            }
            catch (exn){
              if (exn === Caml_builtin_exceptions.not_found) {
                existingConstraints = ConstraintSet.empty;
              } else {
                throw exn;
              }
            }
            var newConstraints = Curry._2(ConstraintSet.add, cellConstraintToAdd, existingConstraints);
            return Curry._3(Engine$ReasonReactExamples.CoordsSetMap.add, intersectingCoords, newConstraints, map);
          }
        }), map, Engine$ReasonReactExamples.CoordsSetMap.empty);
  return Curry._3(Engine$ReasonReactExamples.CoordsSetMap.add, cellConstraintToAdd[/* effectedCells */2], Curry._1(ConstraintSet.singleton, cellConstraintToAdd), map$1);
}

function compoundConstraints(set) {
  return Curry._3(ConstraintSet.fold, addConstraint, set, Engine$ReasonReactExamples.CoordsSetMap.empty);
}

var Constraints = {
  ConstraintWithNoCoords: ConstraintWithNoCoords,
  addConstraint: addConstraint,
  compoundConstraints: compoundConstraints
};

var Group = { };

exports.Constraint = Constraint;
exports.ConstraintSet = ConstraintSet;
exports.Constraints = Constraints;
exports.Group = Group;
/* ConstraintSet Not a pure module */
