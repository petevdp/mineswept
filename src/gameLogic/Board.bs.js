'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Caml_obj = require("bs-platform/lib/js/caml_obj.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Caml_exceptions = require("bs-platform/lib/js/caml_exceptions.js");
var CustomUtils$ReasonReactExamples = require("../utils/CustomUtils.bs.js");

var Cell = { };

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

function makeRaw(param, minedCoords) {
  var xSize = param[0];
  return $$Array.init(param[1], (function (i) {
                return $$Array.init(xSize, (function (j) {
                              var mined = List.exists((function (e) {
                                      return Caml_obj.caml_equal(e, /* tuple */[
                                                  j,
                                                  i
                                                ]);
                                    }), minedCoords);
                              return /* record */[
                                      /* state : Hidden */0,
                                      /* mined */mined
                                    ];
                            }));
              }));
}

function make(size, minedCoords) {
  return CustomUtils$ReasonReactExamples.Matrix.map((function (param, coords) {
                var numAdjacentMines = getNumAdjacent(coords, minedCoords);
                return /* record */[
                        /* state */param[/* state */0],
                        /* mined */param[/* mined */1],
                        /* numAdjacentMines */numAdjacentMines
                      ];
              }), makeRaw(size, minedCoords));
}

var InvalidBoardState = Caml_exceptions.create("Board-ReasonReactExamples.InvalidBoardState");

function getRestrictedModel(board) {
  return CustomUtils$ReasonReactExamples.Matrix.map((function (param, param$1) {
                switch (param[/* state */0]) {
                  case /* Hidden */0 :
                      return /* Hidden */0;
                  case /* Visible */1 :
                      if (param[/* mined */1]) {
                        throw [
                              InvalidBoardState,
                              "the board has a visible cell which has a mine, so it shouldn't be evaluated by an engine"
                            ];
                      }
                      return /* Visible */[param[/* numAdjacentMines */2]];
                  case /* Flagged */2 :
                      return /* Flagged */1;
                  
                }
              }), board);
}

function revealAllMines(board) {
  return CustomUtils$ReasonReactExamples.Matrix.map((function (cell, param) {
                if (cell[/* mined */1]) {
                  return /* record */[
                          /* state : Visible */1,
                          /* mined */cell[/* mined */1],
                          /* numAdjacentMines */cell[/* numAdjacentMines */2]
                        ];
                } else {
                  return cell;
                }
              }), board);
}

function checkAndReveal(coords, board) {
  var y = coords[1];
  var x = coords[0];
  var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
  Caml_array.caml_array_set(Caml_array.caml_array_get(board, y), x, /* record */[
        /* state : Visible */1,
        /* mined */cell[/* mined */1],
        /* numAdjacentMines */cell[/* numAdjacentMines */2]
      ]);
  if (cell[/* numAdjacentMines */2] === 0) {
    var adjacentCoords = getAdjacent(coords, CustomUtils$ReasonReactExamples.Matrix.size(board));
    var hiddenAdjacentCoords = List.filter((function (param) {
              return Caml_array.caml_array_get(Caml_array.caml_array_get(board, param[1]), param[0])[/* state */0] === /* Hidden */0;
            }))(adjacentCoords);
    return Belt_List.reduce(hiddenAdjacentCoords, board, (function (board, coords) {
                  return checkAndReveal(coords, board);
                }));
  } else {
    return board;
  }
}

function hasUnfinishedCells(model) {
  var cellList = CustomUtils$ReasonReactExamples.Matrix.toList(model);
  var hasHiddenCells = List.exists((function (param) {
          return param[/* state */0] === /* Hidden */0;
        }), cellList);
  var hasIncorrectFlaggedCells = List.exists((function (param) {
          if (param[/* mined */1]) {
            return false;
          } else {
            return param[/* state */0] === /* Flagged */2;
          }
        }), cellList);
  if (hasHiddenCells) {
    return true;
  } else {
    return hasIncorrectFlaggedCells;
  }
}

var adjacentCoords = getAdjacent;

exports.Cell = Cell;
exports.Coords = Coords;
exports.adjacentCoords = adjacentCoords;
exports.getAdjacentCells = getAdjacentCells;
exports.makeRaw = makeRaw;
exports.make = make;
exports.InvalidBoardState = InvalidBoardState;
exports.getRestrictedModel = getRestrictedModel;
exports.revealAllMines = revealAllMines;
exports.checkAndReveal = checkAndReveal;
exports.hasUnfinishedCells = hasUnfinishedCells;
/* No side effect */
