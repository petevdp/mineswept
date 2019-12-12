'use strict';

var $$Map = require("bs-platform/lib/js/map.js");
var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Caml_obj = require("bs-platform/lib/js/caml_obj.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_option = require("bs-platform/lib/js/caml_option.js");
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

function orderValue(ordering) {
  switch (ordering) {
    case /* AFirst */0 :
        return 1;
    case /* BFirst */1 :
        return -1;
    case /* Equal */2 :
        return 0;
    
  }
}

function resolveOrdering(orderings) {
  try {
    return List.find((function (ord) {
                  return ord !== /* Equal */2;
                }), orderings);
  }
  catch (exn){
    if (exn === Caml_builtin_exceptions.not_found) {
      return /* Equal */2;
    } else {
      throw exn;
    }
  }
}

function compare(a, b) {
  var match = Caml_obj.caml_compare(a[/* coords */0], b[/* coords */0]);
  var match$1 = a[/* effect */1];
  var match$2 = b[/* effect */1];
  if (match !== 0) {
    return match;
  } else if (match$1) {
    if (match$2) {
      return 0;
    } else {
      return -1;
    }
  } else if (match$2) {
    return 1;
  } else {
    return 0;
  }
}

var Descriminator = {
  compare: compare
};

var DescriminatorSet = $$Set.Make(Descriminator);

function makeBase(coords, boardSize, numMines, board) {
  return List.fold_left((function (group, param) {
                var y = param[1];
                var x = param[0];
                var match = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
                if (typeof match === "number") {
                  if (match !== 0) {
                    return /* record */[
                            /* maxMines */group[/* maxMines */0] - 1 | 0,
                            /* minMines */group[/* minMines */1] - 1 | 0,
                            /* coordsSet */group[/* coordsSet */2],
                            /* descriminatorSet */group[/* descriminatorSet */3]
                          ];
                  } else {
                    return /* record */[
                            /* maxMines */group[/* maxMines */0],
                            /* minMines */group[/* minMines */1],
                            /* coordsSet */Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.add, /* tuple */[
                                  x,
                                  y
                                ], group[/* coordsSet */2]),
                            /* descriminatorSet */group[/* descriminatorSet */3]
                          ];
                  }
                } else {
                  return group;
                }
              }), /* record */[
              /* maxMines */numMines,
              /* minMines */numMines,
              /* coordsSet */CustomUtils$ReasonReactExamples.CoordsSet.empty,
              /* descriminatorSet */Curry._1(DescriminatorSet.singleton, /* record */[
                    /* coords */coords,
                    /* effect : Included */0
                  ])
            ], CustomUtils$ReasonReactExamples.Coords.getAdjacent(coords, boardSize));
}

function numCoords(t) {
  return List.length(Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.elements, t[/* coordsSet */2]));
}

function avgMines(t) {
  var match = t[/* maxMines */0] === t[/* minMines */1];
  if (match) {
    return numCoords(t) / numCoords(t);
  }
  
}

function compare$1(a, b) {
  var lenA = numCoords(a);
  var lenB = numCoords(b);
  var isAFlaggable = lenA === a[/* minMines */1];
  var isBFlaggable = lenB === b[/* minMines */1];
  var flagOrder = isAFlaggable ? (
      isBFlaggable ? /* Equal */2 : /* AFirst */0
    ) : (
      isBFlaggable ? /* BFirst */1 : /* Equal */2
    );
  var isACheckable = a[/* minMines */1] === 0;
  var isBCheckable = b[/* minMines */1] === 0;
  var checkOrder = isACheckable ? (
      isBCheckable ? /* Equal */2 : /* AFirst */0
    ) : (
      isBCheckable ? /* BFirst */1 : /* Equal */2
    );
  var match = avgMines(a);
  var match$1 = avgMines(b);
  var averageOrder;
  if (match !== undefined && match$1 !== undefined) {
    var diff = match - match$1;
    averageOrder = diff > 0.0 ? /* AFirst */0 : (
        diff < 0.0 ? /* BFirst */1 : /* Equal */2
      );
  } else {
    averageOrder = /* Equal */2;
  }
  var priorityOrdering = resolveOrdering(/* :: */[
        flagOrder,
        /* :: */[
          checkOrder,
          /* :: */[
            averageOrder,
            /* [] */0
          ]
        ]
      ]);
  if (priorityOrdering >= 2) {
    return Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.compare, a[/* coordsSet */2], b[/* coordsSet */2]);
  } else {
    return orderValue(priorityOrdering);
  }
}

var Group = {
  Descriminator: Descriminator,
  DescriminatorSet: DescriminatorSet,
  makeBase: makeBase,
  numCoords: numCoords,
  avgMines: avgMines,
  compare: compare$1
};

var GroupSet = $$Set.Make({
      compare: compare$1
    });

var CoordsSetMap = $$Map.Make({
      compare: CustomUtils$ReasonReactExamples.CoordsSet.compare
    });

var merge = Curry._1(CustomUtils$ReasonReactExamples.CoordsMap.merge, (function (param, a, b) {
        if (a !== undefined) {
          var group = Caml_option.valFromOption(a);
          if (b !== undefined) {
            return Caml_option.some(Curry._2(GroupSet.union, group, Caml_option.valFromOption(b)));
          } else {
            return Caml_option.some(group);
          }
        } else if (b !== undefined) {
          return Caml_option.some(Caml_option.valFromOption(b));
        } else {
          return ;
        }
      }));

function invertGroupMap(groups) {
  return Curry._3(CoordsSetMap.fold, (function (coordsSet, group, map) {
                var addMap = Curry._3(CustomUtils$ReasonReactExamples.CoordsSet.fold, (function (coords, map) {
                        return Curry._3(CustomUtils$ReasonReactExamples.CoordsMap.add, coords, Curry._1(GroupSet.singleton, group), map);
                      }), coordsSet, CustomUtils$ReasonReactExamples.CoordsMap.empty);
                return Curry._2(merge, addMap, map);
              }), groups, CustomUtils$ReasonReactExamples.CoordsMap.empty);
}

function make$1(board) {
  var cellList = $$Array.to_list(CustomUtils$ReasonReactExamples.Matrix.flattenWithCoords(board));
  var size = CustomUtils$ReasonReactExamples.Matrix.size(board);
  return invertGroupMap(List.fold_left((function (map, param) {
                    var cell = param[0];
                    if (typeof cell === "number") {
                      return map;
                    } else {
                      var mineCount = cell[0];
                      if (mineCount === 0) {
                        return map;
                      } else {
                        var group = makeBase(param[1], size, mineCount, board);
                        var val;
                        try {
                          val = Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.choose, group[/* coordsSet */2]);
                        }
                        catch (exn){
                          if (exn === Caml_builtin_exceptions.not_found) {
                            return map;
                          } else {
                            throw exn;
                          }
                        }
                        return Curry._3(CoordsSetMap.add, group[/* coordsSet */2], group, map);
                      }
                    }
                  }), CoordsSetMap.empty, cellList));
}

var GroupedCellMap = {
  merge: merge,
  invertGroupMap: invertGroupMap,
  make: make$1
};

var DescriminatorMap = $$Map.Make({
      compare: DescriminatorSet.compare
    });

function getIntersectingCellCoords(a, b) {
  return Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, a[/* coordsSet */2], b[/* coordsSet */2]);
}

function getConstrainedGroups(a, b) {
  var cdsA = a[/* coordsSet */2];
  var cdsB = b[/* coordsSet */2];
  var cdsInter = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.inter, cdsA, cdsB);
  var cdsOnlyA = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.filter, (function (elt) {
          return !Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.mem, elt, cdsB);
        }), cdsA);
  var cdsOnlyB = Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.filter, (function (elt) {
          return !Curry._2(CustomUtils$ReasonReactExamples.CoordsSet.mem, elt, cdsA);
        }), cdsA);
  var minMinesInter = List.fold_left((function (max, num) {
          var match = max > num;
          if (match) {
            return max;
          } else {
            return num;
          }
        }), 0, /* :: */[
        a[/* minMines */1] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, cdsOnlyA) | 0,
        /* :: */[
          b[/* minMines */1] - Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, cdsOnlyB) | 0,
          /* [] */0
        ]
      ]);
  var maxMinesInter = List.fold_left((function (min, num) {
          var match = min < num;
          if (match) {
            return min;
          } else {
            return num;
          }
        }), Curry._1(CustomUtils$ReasonReactExamples.CoordsSet.cardinal, cdsInter), /* :: */[
        a[/* maxMines */0],
        /* :: */[
          b[/* maxMines */0],
          /* [] */0
        ]
      ]);
  var gOnlyA_000 = /* maxMines */a[/* maxMines */0] - maxMinesInter | 0;
  var gOnlyA_001 = /* minMines */a[/* minMines */1] - minMinesInter | 0;
  var gOnlyA_003 = /* descriminatorSet */Curry._2(DescriminatorSet.filter, (function (d) {
          try {
            Curry._2(DescriminatorSet.find, d, b[/* descriminatorSet */3]);
            return false;
          }
          catch (exn){
            if (exn === Caml_builtin_exceptions.not_found) {
              return true;
            } else {
              throw exn;
            }
          }
        }), a[/* descriminatorSet */3]);
  var gOnlyA = /* record */[
    gOnlyA_000,
    gOnlyA_001,
    /* coordsSet */cdsOnlyA,
    gOnlyA_003
  ];
  var gOnlyB_000 = /* maxMines */b[/* maxMines */0] - maxMinesInter | 0;
  var gOnlyB_001 = /* minMines */b[/* minMines */1] - minMinesInter | 0;
  var gOnlyB_003 = /* descriminatorSet */Curry._2(DescriminatorSet.filter, (function (d) {
          try {
            Curry._2(DescriminatorSet.find, d, a[/* descriminatorSet */3]);
            return false;
          }
          catch (exn){
            if (exn === Caml_builtin_exceptions.not_found) {
              return true;
            } else {
              throw exn;
            }
          }
        }), b[/* descriminatorSet */3]);
  var gOnlyB = /* record */[
    gOnlyB_000,
    gOnlyB_001,
    /* coordsSet */cdsOnlyB,
    gOnlyB_003
  ];
  var gInter_003 = /* descriminatorSet */Curry._2(DescriminatorSet.union, b[/* descriminatorSet */3], a[/* descriminatorSet */3]);
  var gInter = /* record */[
    /* maxMines */maxMinesInter,
    /* minMines */minMinesInter,
    /* coordsSet */cdsInter,
    gInter_003
  ];
  return List.fold_left((function (map, group) {
                return Curry._3(DescriminatorMap.add, group[/* descriminatorSet */3], group, map);
              }), DescriminatorMap.empty, /* :: */[
              gOnlyA,
              /* :: */[
                gOnlyB,
                /* :: */[
                  gInter,
                  /* [] */0
                ]
              ]
            ]);
}

var GroupComputations = {
  DescriminatorMap: DescriminatorMap,
  getIntersectingCellCoords: getIntersectingCellCoords,
  getConstrainedGroups: getConstrainedGroups
};

function make$2(coords, boardSize, numMines, board) {
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
                    var group = make$2(coords, size, mineCount, board);
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
  make: make$2,
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
exports.orderValue = orderValue;
exports.resolveOrdering = resolveOrdering;
exports.Group = Group;
exports.GroupSet = GroupSet;
exports.CoordsSetMap = CoordsSetMap;
exports.GroupedCellMap = GroupedCellMap;
exports.GroupComputations = GroupComputations;
exports.BaseGroup = BaseGroup;
exports.solver1 = solver1;
/* DescriminatorSet Not a pure module */
