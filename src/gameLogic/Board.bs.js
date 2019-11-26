'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Caml_obj = require("bs-platform/lib/js/caml_obj.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var CustomUtils$ReasonReactExamples = require("../utils/CustomUtils.bs.js");

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
  adjacentDiff: adjacentDiff,
  getAdjacent: getAdjacent,
  getNumAdjacent: getNumAdjacent
};

function getAdjacentCells(param, matrix) {
  var y = param[1];
  var x = param[0];
  var xSize = Caml_array.caml_array_get(matrix, 0).length;
  var ySize = matrix.length;
  return List.map((function (param) {
                return Caml_array.caml_array_get(Caml_array.caml_array_get(matrix, param[1]), param[0]);
              }), List.filter((function (param) {
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
                      }), adjacentDiff)));
}

function makeRaw(param, minedCells) {
  var xSize = param[0];
  return $$Array.init(param[1], (function (i) {
                return $$Array.init(xSize, (function (j) {
                              var mined = List.exists((function (e) {
                                      return Caml_obj.caml_equal(e, /* tuple */[
                                                  j,
                                                  i
                                                ]);
                                    }), minedCells);
                              return /* record */[
                                      /* state : Hidden */0,
                                      /* mined */mined
                                    ];
                            }));
              }));
}

function make(size, minedCells) {
  return CustomUtils$ReasonReactExamples.Matrix.map((function (param, coords) {
                var numAdjacentMines = getNumAdjacent(coords, minedCells);
                return /* record */[
                        /* state */param[/* state */0],
                        /* mined */param[/* mined */1],
                        /* numAdjacentMines */numAdjacentMines
                      ];
              }), makeRaw(size, minedCells));
}

function initRandom(size, mineCount) {
  var allCoords = CustomUtils$ReasonReactExamples.MyList.combinationRange(size[0], size[1]);
  var shuffled = Belt_List.shuffle(allCoords);
  var minedCells = Belt_List.take(shuffled, mineCount);
  console.log("mined: ");
  if (minedCells !== undefined) {
    var arr = minedCells;
    console.log($$Array.of_list(arr));
    return make(size, arr);
  } else {
    return make(size, /* [] */0);
  }
}

var adjacentCoords = getAdjacent;

exports.Coords = Coords;
exports.adjacentCoords = adjacentCoords;
exports.getAdjacentCells = getAdjacentCells;
exports.makeRaw = makeRaw;
exports.make = make;
exports.initRandom = initRandom;
/* No side effect */
