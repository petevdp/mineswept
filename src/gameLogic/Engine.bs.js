'use strict';

var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
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

function makeMapFromRestrictedBoard(board) {
  return CustomUtils$ReasonReactExamples.Matrix.reduce((function (map, cell, coords) {
                var size = CustomUtils$ReasonReactExamples.Matrix.size(board);
                if (typeof cell === "number") {
                  return map;
                } else {
                  var mineCount = cell[0];
                  if (mineCount !== 0) {
                    var adjacentCoords = CustomUtils$ReasonReactExamples.Coords.getAdjacent(size, coords);
                    var boardConstraint = List.fold_left((function (t, param) {
                            var y = param[1];
                            var x = param[0];
                            var match = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
                            if (typeof match === "number") {
                              if (match !== 0) {
                                return /* record */[
                                        /* mineCount */t[/* mineCount */0] - 1 | 0,
                                        /* originCoords */t[/* originCoords */1],
                                        /* coordsSet */t[/* coordsSet */2],
                                        /* effect */t[/* effect */3]
                                      ];
                              } else {
                                return /* record */[
                                        /* mineCount */t[/* mineCount */0],
                                        /* originCoords */t[/* originCoords */1],
                                        /* coordsSet */Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, /* tuple */[
                                              x,
                                              y
                                            ], t[/* coordsSet */2]),
                                        /* effect */t[/* effect */3]
                                      ];
                              }
                            } else {
                              return t;
                            }
                          }), /* record */[
                          /* mineCount */mineCount,
                          /* originCoords */coords,
                          /* coordsSet */CustomUtils$ReasonReactExamples.CoordsSet.empty,
                          /* effect : Include */0
                        ], adjacentCoords);
                    return Curry._3(CustomUtils$ReasonReactExamples.CoordsMap.add, coords, boardConstraint, map);
                  } else {
                    return map;
                  }
                }
              }), CustomUtils$ReasonReactExamples.CoordsMap.empty, board);
}

function exclude(g) {
  return /* record */[
          /* mineCount */g[/* mineCount */0],
          /* originCoords */g[/* originCoords */1],
          /* coordsSet */g[/* coordsSet */2],
          /* effect : Exclude */1
        ];
}

var BoardConstraint = {
  compare: compare,
  makeMapFromRestrictedBoard: makeMapFromRestrictedBoard,
  exclude: exclude
};

var ConstraintSet = $$Set.Make({
      compare: compare
    });

function excludeConstraints(constraints) {
  return Curry._1(ConstraintSet.of_list, List.map((function (c) {
                    return /* record */[
                            /* mineCount */c[/* mineCount */0],
                            /* originCoords */c[/* originCoords */1],
                            /* coordsSet */c[/* coordsSet */2],
                            /* effect : Exclude */1
                          ];
                  }), Curry._1(ConstraintSet.elements, constraints)));
}

function compare$1(a, b) {
  return Curry._2(ConstraintSet.compare, a[/* constraintSet */2], b[/* constraintSet */2]);
}

function make$1(boardConstraint) {
  var mineCount = boardConstraint[/* mineCount */0];
  return /* record */[
          /* minMines */mineCount,
          /* maxMines */mineCount,
          /* constraintSet */Curry._1(ConstraintSet.singleton, boardConstraint),
          /* coordsSet */boardConstraint[/* coordsSet */2]
        ];
}

function conflate(a, b) {
  var interCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* coordsSet */3], b[/* coordsSet */3]);
  var exclACoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.diff, a[/* coordsSet */3], interCoords);
  var exclBCoords = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.diff, b[/* coordsSet */3], interCoords);
  var interGroup_000 = /* minMines */CustomUtils$ReasonReactExamples.IntUtils.max(/* :: */[
        a[/* minMines */0] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, exclACoords) | 0,
        /* :: */[
          b[/* minMines */0] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, exclBCoords) | 0,
          /* [] */0
        ]
      ]);
  var interGroup_001 = /* maxMines */CustomUtils$ReasonReactExamples.IntUtils.min(/* :: */[
        Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, interCoords),
        /* :: */[
          a[/* maxMines */1],
          /* :: */[
            b[/* maxMines */1],
            /* [] */0
          ]
        ]
      ]);
  var interGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, a[/* constraintSet */2], b[/* constraintSet */2]);
  var interGroup = /* record */[
    interGroup_000,
    interGroup_001,
    interGroup_002,
    /* coordsSet */interCoords
  ];
  var exclAGroup_000 = /* minMines */a[/* minMines */0] - interGroup_001 | 0;
  var exclAGroup_001 = /* maxMines */a[/* maxMines */1] - interGroup_000 | 0;
  var exclAGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, a[/* constraintSet */2], excludeConstraints(b[/* constraintSet */2]));
  var exclAGroup = /* record */[
    exclAGroup_000,
    exclAGroup_001,
    exclAGroup_002,
    /* coordsSet */exclACoords
  ];
  var exclBGroup_000 = /* minMines */b[/* minMines */0] - interGroup_001 | 0;
  var exclBGroup_001 = /* maxMines */b[/* maxMines */1] - interGroup_000 | 0;
  var exclBGroup_002 = /* constraintSet */Curry._2(ConstraintSet.union, b[/* constraintSet */2], excludeConstraints(a[/* constraintSet */2]));
  var exclBGroup = /* record */[
    exclBGroup_000,
    exclBGroup_001,
    exclBGroup_002,
    /* coordsSet */exclBCoords
  ];
  return /* tuple */[
          interGroup,
          exclAGroup,
          exclBGroup
        ];
}

function getActionIfCertain(param) {
  var coordsSet = param[/* coordsSet */3];
  var canCheck = param[/* maxMines */1] === 0;
  var canFlag = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, coordsSet) === param[/* minMines */0];
  var coords = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, coordsSet);
  if (canCheck) {
    return /* Check */Block.__(0, [coords]);
  } else if (canFlag) {
    return /* ToggleFlag */Block.__(1, [coords]);
  } else {
    return ;
  }
}

var Group = {
  compare: compare$1,
  make: make$1,
  conflate: conflate,
  getActionIfCertain: getActionIfCertain
};

var GroupSet = $$Set.Make({
      compare: compare$1
    });

var InvalidConnection = Caml_exceptions.create("Engine-ReasonReactExamples.InvalidConnection");

function applyConstraint(groups, $$const) {
  var constraintGroup = make$1($$const);
  var targetGroup = /* record */[/* contents */constraintGroup];
  var toNormalize = groups;
  var normalizedGroups = /* [] */0;
  var $$break = false;
  while(!$$break) {
    var match = List.partition((function (g) {
            var numInter = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, g[/* coordsSet */3], targetGroup[0][/* coordsSet */3]));
            return numInter === 0;
          }), toNormalize);
    var connected = match[1];
    normalizedGroups = List.concat(/* :: */[
          normalizedGroups,
          /* :: */[
            match[0],
            /* [] */0
          ]
        ]);
    if (connected) {
      var match$1 = conflate(targetGroup[0], connected[0]);
      normalizedGroups = List.concat(/* :: */[
            normalizedGroups,
            /* :: */[
              /* :: */[
                match$1[1],
                /* :: */[
                  match$1[2],
                  /* [] */0
                ]
              ],
              /* [] */0
            ]
          ]);
      targetGroup[0] = match$1[0];
    } else {
      $$break = true;
    }
  };
  return List.concat(/* :: */[
              normalizedGroups,
              /* :: */[
                /* :: */[
                  targetGroup[0],
                  /* [] */0
                ],
                /* [] */0
              ]
            ]);
}

function getActionFromEngine(t, unrestrictedBoard) {
  return Curry._1(t, make(unrestrictedBoard));
}

function random(board) {
  var match = List.hd(List.filter((function (param) {
                return param[0] === /* Hidden */0;
              }))($$Array.to_list(Belt_Array.shuffle(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board)))));
  return /* Check */Block.__(0, [match[1]]);
}

function solver(board) {
  var unAppliedConstraints = Curry._1(CustomUtils$ReasonReactExamples.CoordsMap.bindings, makeMapFromRestrictedBoard(board));
  var normalizedGroups = /* [] */0;
  var action = undefined;
  while(action === undefined) {
    var match = unAppliedConstraints;
    if (match) {
      unAppliedConstraints = match[1];
      normalizedGroups = applyConstraint(normalizedGroups, match[0][1]);
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
        tmp = getActionIfCertain(group);
      }
      action = tmp;
    } else {
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

var registry_000 = /* record */[
  /* name */"solver",
  /* engine */solver
];

var registry_001 = /* :: */[
  /* record */[
    /* name */"random",
    /* engine */random
  ],
  /* [] */0
];

var registry = /* :: */[
  registry_000,
  registry_001
];

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
