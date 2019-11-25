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