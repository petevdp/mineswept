'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
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

function cellCheck(prevPhase, prevBoard, mineCount, coords) {
  var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(prevBoard, coords[1]), coords[0]);
  var match = cell[/* state */0];
  var match$1 = cell[/* mined */1];
  var board = match !== 0 ? prevBoard : (
      match$1 ? Board$ReasonReactExamples.revealAllMines(prevBoard) : Board$ReasonReactExamples.checkAndReveal(coords, prevBoard)
    );
  var allCells = $$Array.to_list(Curry._1(CustomUtils$ReasonReactExamples.Matrix.flatten, board));
  var visibleCells = List.filter((function (param) {
            return param[/* state */0] === /* Visible */1;
          }))(allCells);
  var onlyMinedCellsLeft = (List.length(allCells) - List.length(visibleCells) | 0) === mineCount;
  var match$2 = cell[/* mined */1];
  var phase = typeof prevPhase === "number" ? (
      match$2 ? /* Ended */[/* Loss */1] : (
          onlyMinedCellsLeft ? /* Ended */[/* Win */0] : /* Playing */1
        )
    ) : /* Ended */[prevPhase[0]];
  return /* tuple */[
          board,
          phase
        ];
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

function random(size, mineCount) {
  var allCoords = CustomUtils$ReasonReactExamples.MyList.combinationRange(size[0], size[1]);
  var minedCoords = Belt_List.take(Belt_List.shuffle(allCoords), mineCount);
  if (minedCoords !== undefined) {
    return minedCoords;
  } else {
    return /* [] */0;
  }
}

var MinePopulationStrategy = {
  random: random
};

function reduce(history, action) {
  var prevBoard = List.hd(history);
  var prevBoard$1 = prevBoard[/* board */1];
  var prevPhase = prevBoard[/* phase */0];
  var mineCount = prevBoard[/* mineCount */3];
  var newBoard = $$Array.map($$Array.copy, prevBoard$1);
  var match;
  switch (action.tag | 0) {
    case /* Check */0 :
        match = typeof prevPhase === "number" ? cellCheck(prevPhase, newBoard, mineCount, action[0]) : /* tuple */[
            newBoard,
            /* Ended */[prevPhase[0]]
          ];
        break;
    case /* ToggleFlag */1 :
        match = typeof prevPhase === "number" ? /* tuple */[
            toggleFlag(action[0], prevBoard$1),
            /* Playing */1
          ] : /* tuple */[
            newBoard,
            /* Ended */[prevPhase[0]]
          ];
        break;
    case /* Rewind */2 :
        var steps = action[0];
        var length = List.length(history);
        var match$1 = length > steps;
        var steps$1 = match$1 ? steps : length;
        var match$2 = List.nth(history, steps$1);
        match = /* tuple */[
          match$2[/* board */1],
          match$2[/* phase */0]
        ];
        break;
    
  }
  var newBoard$1 = match[0];
  var flagCount = List.length(List.filter((function (param) {
                return param[/* state */0] === /* Flagged */2;
              }))($$Array.to_list(Curry._1(CustomUtils$ReasonReactExamples.Matrix.flatten, newBoard$1))));
  return /* :: */[
          /* record */[
            /* phase */match[1],
            /* board */newBoard$1,
            /* flagCount */flagCount,
            /* mineCount */mineCount,
            /* lastAction */action
          ],
          history
        ];
}

function make(param) {
  var mineCount = param[/* mineCount */2];
  var size = param[/* size */0];
  return /* record */[
          /* phase : Start */0,
          /* board */Board$ReasonReactExamples.make(size, Curry._2(param[/* minePopulationStrategy */1], size, mineCount)),
          /* flagCount */0,
          /* mineCount */mineCount,
          /* lastAction */undefined
        ];
}

exports.staticCellCheck = staticCellCheck;
exports.cellCheck = cellCheck;
exports.toggleFlag = toggleFlag;
exports.MinePopulationStrategy = MinePopulationStrategy;
exports.reduce = reduce;
exports.make = make;
/* No side effect */
