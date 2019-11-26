'use strict';

var List = require("bs-platform/lib/js/list.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");
var Board$ReasonReactExamples = require("./Board.bs.js");
var CustomUtils$ReasonReactExamples = require("../utils/CustomUtils.bs.js");

function staticCellCheck(state) {
  if (state >= 2) {
    return /* Hidden */0;
  } else {
    return /* Visible */1;
  }
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

function revealCells(coords, board) {
  var y = coords[1];
  var x = coords[0];
  var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
  Caml_array.caml_array_set(Caml_array.caml_array_get(board, y), x, /* record */[
        /* state : Visible */1,
        /* mined */cell[/* mined */1],
        /* numAdjacentMines */cell[/* numAdjacentMines */2]
      ]);
  if (cell[/* numAdjacentMines */2] === 0) {
    var adjacentCoords = Board$ReasonReactExamples.adjacentCoords(coords, CustomUtils$ReasonReactExamples.Matrix.size(board));
    var hiddenAdjacentCoords = List.filter((function (param) {
              return Caml_array.caml_array_get(Caml_array.caml_array_get(board, param[1]), param[0])[/* state */0] === /* Hidden */0;
            }))(adjacentCoords);
    return Belt_List.reduce(hiddenAdjacentCoords, board, (function (board, coords) {
                  return revealCells(coords, board);
                }));
  } else {
    return board;
  }
}

function cellCheck(coords, gameState, board) {
  var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(board, coords[0]), coords[1]);
  console.log("checking cell");
  console.log(coords);
  console.log(cell);
  var match = cell[/* state */0];
  var match$1 = cell[/* mined */1];
  if (gameState >= 2) {
    return /* tuple */[
            board,
            /* Ended */2
          ];
  } else if (match !== 0) {
    return /* tuple */[
            board,
            gameState
          ];
  } else if (match$1) {
    return /* tuple */[
            revealAllMines(board),
            /* Ended */2
          ];
  } else {
    return /* tuple */[
            revealCells(coords, board),
            /* Playing */1
          ];
  }
}

function toggleFlag(param, board) {
  var y = param[1];
  var x = param[0];
  var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
  var match = cell[/* state */0];
  var newState;
  switch (match) {
    case /* Hidden */0 :
        newState = /* Flagged */2;
        break;
    case /* Visible */1 :
        newState = /* Visible */1;
        break;
    case /* Flagged */2 :
        newState = /* Hidden */0;
        break;
    
  }
  Caml_array.caml_array_set(Caml_array.caml_array_get(board, y), x, /* record */[
        /* state */newState,
        /* mined */cell[/* mined */1],
        /* numAdjacentMines */cell[/* numAdjacentMines */2]
      ]);
  return board;
}

function update(action, board, gameState, initBoard) {
  console.log("gamestate: ");
  console.log(gameState);
  if (typeof action === "number") {
    return /* tuple */[
            Curry._1(initBoard, /* () */0),
            /* New */0
          ];
  } else if (action.tag) {
    return /* tuple */[
            toggleFlag(action[0], board),
            /* Playing */1
          ];
  } else {
    return cellCheck(action[0], gameState, board);
  }
}

exports.staticCellCheck = staticCellCheck;
exports.revealAllMines = revealAllMines;
exports.revealCells = revealCells;
exports.cellCheck = cellCheck;
exports.toggleFlag = toggleFlag;
exports.update = update;
/* No side effect */
