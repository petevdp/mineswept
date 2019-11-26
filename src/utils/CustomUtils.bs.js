'use strict';

var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");

function map(func, matrix) {
  return $$Array.mapi((function (i, row) {
                return $$Array.mapi((function (j, e) {
                              return Curry._2(func, e, /* tuple */[
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

var Matrix = {
  map: map,
  size: size
};

exports.Matrix = Matrix;
/* No side effect */
