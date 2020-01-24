open GlobalTypes;

type action =
  // board, minecount
  | Check(coords)
  | ToggleFlag(coords)
  | Rewind(int);

type endState =
  | Win
  | Loss;

type phase =
  | Start
  | Playing
  | Ended(endState);
