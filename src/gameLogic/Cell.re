type state =
  | Hidden
  | Visible
  | Flagged;

type model = {
  state,
  mined: bool,
};

type action =
  | Check
  | ToggleFlag;

let update = (action: action, model: model) => {
  let {state} = model;

  let newState: state =
    switch (action, state) {
    | (Check, Hidden) => Visible
    | (Check, Visible) => Visible
    | (Check, Flagged) => Flagged
    | (ToggleFlag, Hidden) => Flagged
    | (ToggleFlag, Visible) => Visible
    | (ToggleFlag, Flagged) => Hidden
    };

  {...model, state: newState};
};