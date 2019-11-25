'use strict';

var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");

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

var Matrix = {
  map: map
};

exports.Matrix = Matrix;
/* No side effect */
