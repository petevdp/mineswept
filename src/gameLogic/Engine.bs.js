'use strict';

var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");
var Caml_chrome_debugger = require("bs-platform/lib/js/caml_chrome_debugger.js");
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
                      return /* Visible */Caml_chrome_debugger.simpleVariant("Visible", [param[/* numAdjacentMines */2]]);
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
  var num = CustomUtils$ReasonReactExamples.Coords.compare(a[/* originCoords */1], b[/* originCoords */1]);
  if (num !== 0) {
    return num;
  } else {
    var match = a[/* effect */3] === b[/* effect */3];
    if (match) {
      return 0;
    } else {
      return 1;
    }
  }
}

function exclude(g) {
  return /* record */Caml_chrome_debugger.record([
            "mineCount",
            "originCoords",
            "coordsSet",
            "effect"
          ], [
            g[/* mineCount */0],
            g[/* originCoords */1],
            g[/* coordsSet */2],
            1
          ]);
}

function make$1(originCoords, mineCount, board) {
  var coordsSet = /* record */Caml_chrome_debugger.record(["contents"], [CustomUtils$ReasonReactExamples.CoordsSet.empty]);
  var mineCount$1 = /* record */Caml_chrome_debugger.record(["contents"], [mineCount]);
  console.log(/* array */[
        "Origin: ",
        String(originCoords[0]),
        String(originCoords[1])
      ]);
  console.log("board: ");
  console.log(board);
  var adjacent = List.map((function (param) {
          var y = param[1];
          var x = param[0];
          return /* tuple */[
                  Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x),
                  /* tuple */[
                    x,
                    y
                  ]
                ];
        }), CustomUtils$ReasonReactExamples.Coords.getAdjacent(originCoords, CustomUtils$ReasonReactExamples.Matrix.size(board)));
  List.iter((function (param) {
          var cell = param[0];
          if (typeof cell === "number") {
            if (cell !== 0) {
              mineCount$1[0] = mineCount$1[0] - 1 | 0;
              return /* () */0;
            } else {
              coordsSet[0] = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, param[1], coordsSet[0]);
              return /* () */0;
            }
          } else {
            return /* () */0;
          }
        }), adjacent);
  console.log("adjacent: ");
  console.log($$Array.of_list(CustomUtils$ReasonReactExamples.Coords.getAdjacent(originCoords, CustomUtils$ReasonReactExamples.Matrix.size(board))));
  console.log("hidden:");
  console.log($$Array.of_list(Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.elements, coordsSet[0])));
  return /* record */Caml_chrome_debugger.record([
            "mineCount",
            "originCoords",
            "coordsSet",
            "effect"
          ], [
            mineCount$1[0],
            originCoords,
            coordsSet[0],
            0
          ]);
}

function makeListFromRestrictedBoard(board) {
  var constraints = /* record */Caml_chrome_debugger.record(["contents"], [0]);
  List.iter((function (param) {
          var cell = param[0];
          if (typeof cell === "number") {
            return /* () */0;
          } else {
            var newConstraint = make$1(param[1], cell[0], board);
            if (newConstraint[/* mineCount */0] > 0) {
              constraints[0] = /* :: */Caml_chrome_debugger.simpleVariant("::", [
                  newConstraint,
                  constraints[0]
                ]);
              return /* () */0;
            } else {
              return 0;
            }
          }
        }), $$Array.to_list(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board)));
  return constraints[0];
}

var BoardConstraint = {
  compare: compare,
  exclude: exclude,
  make: make$1,
  makeListFromRestrictedBoard: makeListFromRestrictedBoard
};

var ConstraintSet = $$Set.Make({
      compare: compare
    });

function excludeConstraints(constraints) {
  return Curry._1(ConstraintSet.of_list, List.map((function (c) {
                    return /* record */Caml_chrome_debugger.record([
                              "mineCount",
                              "originCoords",
                              "coordsSet",
                              "effect"
                            ], [
                              c[/* mineCount */0],
                              c[/* originCoords */1],
                              c[/* coordsSet */2],
                              1
                            ]);
                  }), Curry._1(ConstraintSet.elements, constraints)));
}

function compare$1(a, b) {
  return Curry._2(ConstraintSet.compare, a[/* constraintSet */2], b[/* constraintSet */2]);
}

function make$2(boardConstraint) {
  var mineCount = boardConstraint[/* mineCount */0];
  return /* record */Caml_chrome_debugger.record([
            "minMines",
            "maxMines",
            "constraintSet",
            "coordsSet"
          ], [
            mineCount,
            mineCount,
            Curry._1(ConstraintSet.singleton, boardConstraint),
            boardConstraint[/* coordsSet */2]
          ]);
}

function isEmpty(t) {
  return Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.is_empty, t[/* coordsSet */3]);
}

function conflate(a, b) {
  var interCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* coordsSet */3], b[/* coordsSet */3]);
  var exclACoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.diff, a[/* coordsSet */3], interCoords);
  var exclBCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.diff, b[/* coordsSet */3], interCoords);
  var interGroup_000 = /* minMines */CustomUtils$ReasonReactExamples.IntUtils.max(/* :: */Caml_chrome_debugger.simpleVariant("::", [
          a[/* minMines */0] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, exclACoords) | 0,
          /* :: */Caml_chrome_debugger.simpleVariant("::", [
              b[/* minMines */0] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, exclBCoords) | 0,
              /* :: */Caml_chrome_debugger.simpleVariant("::", [
                  0,
                  /* [] */0
                ])
            ])
        ]));
  var interGroup_001 = /* maxMines */CustomUtils$ReasonReactExamples.IntUtils.min(/* :: */Caml_chrome_debugger.simpleVariant("::", [
          Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, interCoords),
          /* :: */Caml_chrome_debugger.simpleVariant("::", [
              a[/* maxMines */1],
              /* :: */Caml_chrome_debugger.simpleVariant("::", [
                  b[/* maxMines */1],
                  /* [] */0
                ])
            ])
        ]));
  var interGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, a[/* constraintSet */2], b[/* constraintSet */2]);
  var interGroup = /* record */Caml_chrome_debugger.record([
      "minMines",
      "maxMines",
      "constraintSet",
      "coordsSet"
    ], [
      interGroup_000,
      interGroup_001,
      interGroup_002,
      interCoords
    ]);
  var exclAGroup_000 = /* minMines */a[/* minMines */0] - interGroup_001 | 0;
  var exclAGroup_001 = /* maxMines */a[/* maxMines */1] - interGroup_000 | 0;
  var exclAGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, a[/* constraintSet */2], excludeConstraints(b[/* constraintSet */2]));
  var exclAGroup = /* record */Caml_chrome_debugger.record([
      "minMines",
      "maxMines",
      "constraintSet",
      "coordsSet"
    ], [
      exclAGroup_000,
      exclAGroup_001,
      exclAGroup_002,
      exclACoords
    ]);
  var exclBGroup_000 = /* minMines */b[/* minMines */0] - interGroup_001 | 0;
  var exclBGroup_001 = /* maxMines */b[/* maxMines */1] - interGroup_000 | 0;
  var exclBGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, b[/* constraintSet */2], excludeConstraints(a[/* constraintSet */2]));
  var exclBGroup = /* record */Caml_chrome_debugger.record([
      "minMines",
      "maxMines",
      "constraintSet",
      "coordsSet"
    ], [
      exclBGroup_000,
      exclBGroup_001,
      exclBGroup_002,
      exclBCoords
    ]);
  return /* tuple */[
          exclAGroup,
          interGroup,
          exclBGroup
        ];
}

function getActionIfCertain(param) {
  var coordsSet = param[/* coordsSet */3];
  var canCheck = param[/* maxMines */1] === 0;
  var canFlag = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, coordsSet) === param[/* minMines */0];
  var coords = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, coordsSet);
  if (canCheck) {
    return /* Check */Caml_chrome_debugger.variant("Check", 0, [coords]);
  } else if (canFlag) {
    return /* ToggleFlag */Caml_chrome_debugger.variant("ToggleFlag", 1, [coords]);
  } else {
    return ;
  }
}

var Group = {
  compare: compare$1,
  make: make$2,
  isEmpty: isEmpty,
  conflate: conflate,
  getActionIfCertain: getActionIfCertain
};

var GroupSet = $$Set.Make({
      compare: compare$1
    });

var InvalidConnection = Caml_exceptions.create("Engine-ReasonReactExamples.InvalidConnection");

function applyConstraint(groups, $$const) {
  var constraintGroup = make$2($$const);
  var targetGroup = /* record */Caml_chrome_debugger.record(["contents"], [constraintGroup]);
  var toNormalize = groups;
  var normalizedGroups = /* [] */0;
  var $$break = false;
  while(!$$break) {
    var match = List.partition((function (g) {
            var numInter = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, g[/* coordsSet */3], targetGroup[0][/* coordsSet */3]));
            return numInter === 0;
          }), toNormalize);
    var connected = match[1];
    normalizedGroups = List.concat(/* :: */Caml_chrome_debugger.simpleVariant("::", [
            normalizedGroups,
            /* :: */Caml_chrome_debugger.simpleVariant("::", [
                match[0],
                /* [] */0
              ])
          ]));
    if (connected) {
      var match$1 = conflate(targetGroup[0], connected[0]);
      var nonEmpty = List.filter(isEmpty)(/* :: */Caml_chrome_debugger.simpleVariant("::", [
              match$1[1],
              /* :: */Caml_chrome_debugger.simpleVariant("::", [
                  match$1[2],
                  /* [] */0
                ])
            ]));
      normalizedGroups = List.concat(/* :: */Caml_chrome_debugger.simpleVariant("::", [
              normalizedGroups,
              /* :: */Caml_chrome_debugger.simpleVariant("::", [
                  nonEmpty,
                  /* [] */0
                ])
            ]));
      targetGroup[0] = match$1[0];
      toNormalize = connected[1];
    } else {
      $$break = true;
    }
  };
  var normalizedGroups$1 = normalizedGroups;
  var targetGroup$1 = targetGroup[0];
  var match$2 = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.is_empty, targetGroup$1[/* coordsSet */3]);
  if (match$2) {
    return normalizedGroups$1;
  } else {
    return List.concat(/* :: */Caml_chrome_debugger.simpleVariant("::", [
                  normalizedGroups$1,
                  /* :: */Caml_chrome_debugger.simpleVariant("::", [
                      /* :: */Caml_chrome_debugger.simpleVariant("::", [
                          targetGroup$1,
                          /* [] */0
                        ]),
                      /* [] */0
                    ])
                ]));
  }
}

function getActionFromEngine(t, unrestrictedBoard) {
  var restrictedBoard = make(unrestrictedBoard);
  var action = Curry._1(t, restrictedBoard);
  switch (action.tag | 0) {
    case /* Check */0 :
        var match = action[0];
        console.log("Check: " + (String(match[0]) + (" " + String(match[1]))));
        break;
    case /* ToggleFlag */1 :
        var match$1 = action[0];
        console.log("Flag: " + (String(match$1[0]) + (" " + String(match$1[1]))));
        break;
    case /* Rewind */2 :
        break;
    
  }
  return action;
}

function random(board) {
  var match = List.hd(List.filter((function (param) {
                return param[0] === /* Hidden */0;
              }))($$Array.to_list(Belt_Array.shuffle(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board)))));
  return /* Check */Caml_chrome_debugger.variant("Check", 0, [match[1]]);
}

function solver(board) {
  var unAppliedConstraints = makeListFromRestrictedBoard(board);
  var normalizedGroups = /* [] */0;
  var action = undefined;
  while(action === undefined) {
    var match = unAppliedConstraints;
    if (match) {
      unAppliedConstraints = match[1];
      normalizedGroups = applyConstraint(normalizedGroups, match[0]);
      var tmp;
      var exit = 0;
      var group;
      try {
        group = List.find((function (g) {
                return getActionIfCertain(g) !== undefined;
              }), normalizedGroups);
        exit = 1;
      }
      catch (exn){
        if (exn === Caml_builtin_exceptions.not_found) {
          tmp = undefined;
        } else {
          throw exn;
        }
      }
      if (exit === 1) {
        var a = getActionIfCertain(group);
        if (a !== undefined) {
          console.log("making certain move");
        } else {
          console.log("couldn't find move");
        }
        tmp = a;
      }
      action = tmp;
    } else {
      console.log("making random move");
      action = random(board);
    }
  };
  var match$1 = action;
  if (match$1 !== undefined) {
    return match$1;
  } else {
    throw Caml_builtin_exceptions.not_found;
  }
}

var registry_000 = /* record */Caml_chrome_debugger.record([
    "name",
    "engine"
  ], [
    "solver",
    solver
  ]);

var registry_001 = /* :: */Caml_chrome_debugger.simpleVariant("::", [
    /* record */Caml_chrome_debugger.record([
        "name",
        "engine"
      ], [
        "random",
        random
      ]),
    /* [] */0
  ]);

var registry = /* :: */Caml_chrome_debugger.simpleVariant("::", [
    registry_000,
    registry_001
  ]);

exports.RestrictedBoard = RestrictedBoard;
exports.BoardConstraint = BoardConstraint;
exports.ConstraintSet = ConstraintSet;
exports.excludeConstraints = excludeConstraints;
exports.Group = Group;
exports.GroupSet = GroupSet;
exports.InvalidConnection = InvalidConnection;
exports.applyConstraint = applyConstraint;
exports.getActionFromEngine = getActionFromEngine;
exports.random = random;
exports.solver = solver;
exports.registry = registry;
/* ConstraintSet Not a pure module */
