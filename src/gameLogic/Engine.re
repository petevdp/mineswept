open CustomUtils;
open GlobalTypes;

module RestrictedBoard = {
  /** what a player would see */
  type rCell =
    | Hidden
    | Visible(int) // int: num of adjacent mines
    | Flagged;

  type t = matrix(rCell);
  type cellList = list((rCell, coords));

  exception InvalidBoard(string);

  let make = (board: Board.model): t =>
    if (Board.hasVisibleMines(board)) {
      raise(InvalidBoard("the board has visible mines!"));
    } else {
      Matrix.map(
        ~f=
          ({state, numAdjacentMines}: Board.Cell.model, _) => {
            let rCell: rCell =
              switch (state) {
              | Hidden => Hidden
              | Visible => Visible(numAdjacentMines)
              | Flagged => Flagged
              };
            rCell;
          },
        board,
      );
    };
};

module InterpolatedBoard = {
  type t = {
    /**  mine probablities expressed with floats
     *- should probably use ratios but couldn't find anything
     *  for bucklescript */
    probabilities: matrix(option(float)),
    constraints: CoordsMap.t(int),
  };

  let make = (inputBoard: RestrictedBoard.t): t => {
    let probs = Matrix.make(Matrix.size(inputBoard), None);
    Matrix.reduce(
      ~f=
        ({probabilities, constraints}, cell: RestrictedBoard.rCell, coords) => {
          let (value, mineConstraint) =
            switch (cell) {
            | Hidden => (None, None)
            | Flagged => (None, None)
            | Visible(int) => (None, Some(int))
            };
          let (x, y) = coords;
          probabilities[y][x] = value;

          let constraints =
            switch (mineConstraint) {
            | None => constraints
            | Some(num) => CoordsMap.add(coords, num, constraints)
            };

          {probabilities, constraints};
        },
      {probabilities: probs, constraints: CoordsMap.empty},
      inputBoard,
    );
  };

  type probForCell = {
    mineChance: float,
    coords: option(coords),
  };
  /**
   * get the smallest and largest value in mine model
   */
  let maxMin = (model: t): (probForCell, probForCell) => {
    Matrix.reduce(
      ~f=
        ((max, min), value, coords) =>
          switch (value) {
          | None => (max, min)
          | Some(value) when value > max.mineChance => (
              {mineChance: value, coords: Some(coords)},
              min,
            )
          | Some(value) when value < min.mineChance => (
              max,
              {mineChance: value, coords: Some(coords)},
            )
          | _ => (max, min)
          },
      (
        {mineChance: 0.0, coords: None},
        {mineChance: infinity, coords: None},
      ),
      model.probabilities,
    );
  };

  type action =
    | ReportBestMove(Game.action)
    | ApplyConstraint(CoordsMap.t(float), coords);

  let getAction = (model: t): action => {
    // TODO optomize where we check for new maxes and mins based on last action
    let (max, min) = maxMin(model);
    let {constraints, probabilities} = model;

    switch (CoordsMap.choose(constraints), max, min) {
      // no constraints left, so we neeed to choose something
    | exception Not_found =>
      // this might not be the best formula for deciding which is safer
      let minIsSafer = min.mineChance < 1.0 - max.mineChance;
      ReportBestMove(minIsSafer ? ToggleFlag(min.coords) : Check(max.coords));

      // if we get a guaranteed mine/empty, act on it
    | (_, {mineChance: 1.0, coords: coords}, _) => ReportBestMove(Check(coords))
    | (_, _, {mineChance: 0.0, coords: coords}) => ReportBestMove(ToggleFLag(coords))

    // else continue apply constraints
    | (cellConstraint, _, _) => ApplyConstraint(cellConstraint)
  };

  /** Apply some additional info to the board */
  let reduce = (model: t, additions: CoordsMap.t(float)): model => { };

  /**  */
  let buildUntilCertaintyReached =
      (inputBoard: RestrictedBoard.t): option(Game.action) => {
    let model = ref(make(inputBoard));
    let action = ref(interpolate(model));

    // while (action^ != ReportBestMove) {
    //   model := switch(action^) {
    //     | Reduce(map) =>
    //   }
    // }

    action;
  };
};