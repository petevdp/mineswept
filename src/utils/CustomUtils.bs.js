'use strict';

var $$Map = require("bs-platform/lib/js/map.js");
var $$Set = require("bs-platform/lib/js/set.js");
var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Int32 = require("bs-platform/lib/js/int32.js");
var $$String = require("bs-platform/lib/js/string.js");
var Caml_obj = require("bs-platform/lib/js/caml_obj.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");

var BadComparison = Caml_exceptions.create("CustomUtils-ReasonReactExamples.Coords.BadComparison");

function compare(param, param$1) {
  var higherRow = Int32.compare(param[1], param$1[1]);
  var higherCol = Int32.compare(param[0], param$1[0]);
  switch (higherRow) {
    case -1 :
        return -1;
    case 0 :
        var switcher = higherCol + 1 | 0;
        if (switcher > 2 || switcher < 0) {
          throw [
                BadComparison,
                "comparison switch got an impossible value!"
              ];
        }
        return switcher - 1 | 0;
    case 1 :
        return 1;
    default:
      throw [
            BadComparison,
            "comparison switch got an impossible value!"
          ];
  }
}

var adjacentDiff = /* :: */[
  /* tuple */[
    1,
    1
  ],
  /* :: */[
    /* tuple */[
      1,
      0
    ],
    /* :: */[
      /* tuple */[
        1,
        -1
      ],
      /* :: */[
        /* tuple */[
          0,
          1
        ],
        /* :: */[
          /* tuple */[
            0,
            -1
          ],
          /* :: */[
            /* tuple */[
              -1,
              1
            ],
            /* :: */[
              /* tuple */[
                -1,
                0
              ],
              /* :: */[
                /* tuple */[
                  -1,
                  -1
                ],
                /* [] */0
              ]
            ]
          ]
        ]
      ]
    ]
  ]
];

function getAdjacent(param, param$1) {
  var ySize = param$1[1];
  var xSize = param$1[0];
  var y = param[1];
  var x = param[0];
  return List.filter((function (param) {
                  var y = param[1];
                  var x = param[0];
                  if (x >= 0 && x < xSize && y >= 0) {
                    return y < ySize;
                  } else {
                    return false;
                  }
                }))(List.map((function (param) {
                    return /* tuple */[
                            x + param[0] | 0,
                            y + param[1] | 0
                          ];
                  }), adjacentDiff));
}

function getNumAdjacent(param, countedCoords) {
  var y = param[1];
  var x = param[0];
  return List.length(List.filter((function (coords) {
                      return List.exists((function (checked) {
                                    return Caml_obj.caml_equal(checked, coords);
                                  }), countedCoords);
                    }))(List.map((function (param) {
                        return /* tuple */[
                                x + param[0] | 0,
                                y + param[1] | 0
                              ];
                      }), adjacentDiff)));
}

var Coords = {
  BadComparison: BadComparison,
  compare: compare,
  adjacentDiff: adjacentDiff,
  getAdjacent: getAdjacent,
  getNumAdjacent: getNumAdjacent
};

function map(f, matrix) {
  return $$Array.mapi((function (i, row) {
                return $$Array.mapi((function (j, e) {
                              return Curry._2(f, e, /* tuple */[
                                          j,
                                          i
                                        ]);
                            }), row);
              }), matrix);
}

function size(matrix) {
  if (matrix.length === 0 || Caml_array.caml_array_get(matrix, 0).length === 0) {
    return /* tuple */[
            0,
            0
          ];
  } else {
    return /* tuple */[
            Caml_array.caml_array_get(matrix, 0).length,
            matrix.length
          ];
  }
}

function make(param, cell) {
  return $$Array.make_matrix(param[1], param[0], cell);
}

function toList(matrix) {
  return $$Array.to_list(Belt_Array.concatMany(matrix));
}

function flattenWithCoords(matrix) {
  return Belt_Array.concatMany(map((function (cell, coords) {
                    return /* tuple */[
                            cell,
                            coords
                          ];
                  }), matrix));
}

function reduce(f, acc, matrix) {
  return $$Array.fold_left((function (acc, param) {
                return Curry._3(f, acc, param[0], param[1]);
              }), acc, flattenWithCoords(matrix));
}

function copy(matrix) {
  return $$Array.map($$Array.copy, matrix);
}

function select(f, matrix) {
  return reduce((function (acc, a, param) {
                var match = Curry._1(f, a);
                return List.concat(/* :: */[
                            acc,
                            /* :: */[
                              match ? /* :: */[
                                  a,
                                  /* [] */0
                                ] : /* [] */0,
                              /* [] */0
                            ]
                          ]);
              }), /* [] */0, matrix);
}

var Matrix = {
  map: map,
  size: size,
  make: make,
  flatten: Belt_Array.concatMany,
  toList: toList,
  flattenWithCoords: flattenWithCoords,
  reduce: reduce,
  copy: copy,
  select: select
};

var NoNumbersProvided = Caml_exceptions.create("CustomUtils-ReasonReactExamples.IntUtils.NoNumbersProvided");

function max(nums) {
  if (nums) {
    var rest = nums[1];
    var num = nums[0];
    if (rest) {
      return List.fold_left((function (a, b) {
                    var match = a > b;
                    if (match) {
                      return a;
                    } else {
                      return b;
                    }
                  }), num, rest);
    } else {
      return num;
    }
  } else {
    throw NoNumbersProvided;
  }
}

function min(nums) {
  if (nums) {
    var rest = nums[1];
    var num = nums[0];
    if (rest) {
      return List.fold_left((function (a, b) {
                    var match = a < b;
                    if (match) {
                      return a;
                    } else {
                      return b;
                    }
                  }), num, rest);
    } else {
      return num;
    }
  } else {
    throw NoNumbersProvided;
  }
}

var IntUtils = {
  NoNumbersProvided: NoNumbersProvided,
  max: max,
  min: min
};

var CoordsSet = $$Set.Make({
      compare: compare
    });

var CoordsMap = $$Map.Make({
      compare: compare
    });

var StrMap = $$Map.Make({
      compare: $$String.compare
    });

function combinationRange(a, b) {
  return List.concat(Belt_List.makeBy(a, (function (i) {
                    return Belt_List.makeBy(b, (function (j) {
                                  return /* tuple */[
                                          j,
                                          i
                                        ];
                                }));
                  })));
}

var MyList = {
  combinationRange: combinationRange
};

function getRange(size) {
  var coordsSet = /* record */[/* contents */CoordsSet.empty];
  var ySize = size[1];
  for(var i = 0 ,i_finish = size[0]; i <= i_finish; ++i){
    for(var j = 0; j <= ySize; ++j){
      coordsSet[0] = Curry._2(CoordsSet.add, /* tuple */[
            i,
            j
          ], coordsSet[0]);
    }
  }
  return coordsSet;
}

exports.Coords = Coords;
exports.Matrix = Matrix;
exports.IntUtils = IntUtils;
exports.CoordsSet = CoordsSet;
exports.CoordsMap = CoordsMap;
exports.StrMap = StrMap;
exports.MyList = MyList;
exports.getRange = getRange;
/* CoordsSet Not a pure module */
