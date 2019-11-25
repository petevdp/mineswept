'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");

function staticCellCheck(state) {
  if (state >= 2) {
    return /* Hidden */0;
  } else {
    return /* Visible */1;
  }
}

function toggleFlag(state) {
  switch (state) {
    case /* Hidden */0 :
        return /* Flagged */2;
    case /* Visible */1 :
        return /* Visible */1;
    case /* Flagged */2 :
        return /* Hidden */0;
    
  }
}

function update(action, board, initBoard) {
  if (typeof action === "number") {
    return Curry._1(initBoard, /* () */0);
  } else if (action.tag) {
    var match = action[0];
    var y = match[1];
    var x = match[0];
    var cell = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y), x);
    Caml_array.caml_array_set(Caml_array.caml_array_get(board, y), x, /* record */[
          /* state */toggleFlag(cell[/* state */0]),
          /* mined */cell[/* mined */1],
          /* numAdjacentMines */cell[/* numAdjacentMines */2]
        ]);
    return board;
  } else {
    var match$1 = action[0];
    var y$1 = match$1[1];
    var x$1 = match$1[0];
    var cell$1 = Caml_array.caml_array_get(Caml_array.caml_array_get(board, y$1), x$1);
    Caml_array.caml_array_set(Caml_array.caml_array_get(board, y$1), x$1, /* record */[
          /* state */staticCellCheck(cell$1[/* state */0]),
          /* mined */cell$1[/* mined */1],
          /* numAdjacentMines */cell$1[/* numAdjacentMines */2]
        ]);
    return board;
  }
}

exports.staticCellCheck = staticCellCheck;
exports.toggleFlag = toggleFlag;
exports.update = update;
/* No side effect */
