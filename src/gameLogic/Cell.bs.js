'use strict';


function update(action, model) {
  var state = model[/* state */0];
  var newState;
  if (action) {
    switch (state) {
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
  } else {
    newState = state >= 2 ? /* Flagged */2 : /* Visible */1;
  }
  return /* record */[
          /* state */newState,
          /* mined */model[/* mined */1]
        ];
}

exports.update = update;
/* No side effect */
