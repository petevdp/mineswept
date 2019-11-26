'use strict';

var List = require("bs-platform/lib/js/list.js");
var $$Array = require("bs-platform/lib/js/array.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Belt_List = require("bs-platform/lib/js/belt_List.js");
var Caml_array = require("bs-platform/lib/js/caml_array.js");

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

var Matrix = {
  map: map,
  size: size
};

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

exports.Matrix = Matrix;
exports.MyList = MyList;
/* No side effect */
