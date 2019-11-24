'use strict';

var $$Array = require("bs-platform/lib/js/array.js");
var React = require("react");
var ReactDom = require("react-dom");
var Board$ReasonReactExamples = require("./components/Board/Board.bs.js");
var ExampleStyles$ReasonReactExamples = require("./exampleComponents/ExampleStyles.bs.js");

var style = document.createElement("style");

document.head.appendChild(style);

style.innerHTML = ExampleStyles$ReasonReactExamples.style;

function makeContainer(text) {
  var container = document.createElement("div");
  container.className = "container";
  var title = document.createElement("div");
  title.className = "containerTitle";
  title.innerText = text;
  var content = document.createElement("div");
  content.className = "containerContent";
  container.appendChild(title);
  container.appendChild(content);
  document.body.appendChild(container);
  return content;
}

var visibleCell = /* record */[
  /* state : Visible */1,
  /* mined */true
];

var matrix = $$Array.make_matrix(10, 10, visibleCell);

var testMatrix = /* array */[/* array */[
    /* record */[
      /* state : Hidden */0,
      /* mined */false
    ],
    /* record */[
      /* state : Visible */1,
      /* mined */true
    ],
    /* record */[
      /* state : Visible */1,
      /* mined */false
    ],
    /* record */[
      /* state : Flagged */2,
      /* mined */false
    ]
  ]];

ReactDom.render(React.createElement(Board$ReasonReactExamples.make, {
          cellModelMatrix: testMatrix
        }), makeContainer("Cells"));

ReactDom.render(React.createElement(Board$ReasonReactExamples.make, {
          cellModelMatrix: matrix
        }), makeContainer("minesweeper"));

exports.style = style;
exports.makeContainer = makeContainer;
exports.visibleCell = visibleCell;
exports.matrix = matrix;
exports.testMatrix = testMatrix;
/* style Not a pure module */
