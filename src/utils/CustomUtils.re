open GlobalTypes;

module Matrix = {
  let map = (~f: ('a, coords) => 'b, matrix: matrix('a)) =>
    matrix
    |> Array.mapi((i, row) => Array.mapi((j, e: 'a) => f(e, (j, i)), row));

  let size = (matrix: matrix('a)) =>
    if (Array.length(matrix) == 0 || Array.length(matrix[0]) == 0) {
      (0, 0);
    } else {
      (Array.length(matrix[0]), Array.length(matrix));
    };

  let flatten = Belt.Array.concatMany;

  let toList = (matrix: matrix('a)) => matrix->flatten->Array.to_list;

  let flattenWithCoords = (matrix: matrix('a)): array(('a, coords)) =>
    matrix
    |> map(~f=(cell, coords) => (cell, coords))
    |> Belt.Array.concatMany;

  let reduce = (~acc: 'a, ~f: ('a, 'b) => 'a, matrix: matrix('b)): 'a => {
    let cells = flatten(matrix);
    Belt.Array.reduce(cells, acc, f);
  };

  let copy = matrix => Array.map(row => Array.copy(row), matrix);
  // let forEach = (~f: (~coords: coords, 'a`) => unit, matrix: t('a)) => {
  //   let (sizeX, sizeY) = size(matrix);
  //   for (i in 0 to sizeY) {
  //     for (j in 0 to sizex)
  //   };
  // }

  // get a list of cells and their coordinates based on predicate ~f
  let select = (~f: 'a => bool, matrix: matrix('a)) =>
    reduce(
      ~acc=[],
      ~f=(acc, a) => List.concat([acc, f(a) ? [a] : []]),
      matrix,
    );
};

module Coords = {
  type t = (int, int);
  exception BadComparison(string);
  let compare = ((ax, ay), (bx, by)) => {
    let higherRow = Int32.compare(Int32.of_int(ay), Int32.of_int(by));
    let higherCol = Int32.compare(Int32.of_int(ax), Int32.of_int(bx));

    switch (higherRow, higherCol) {
    | (1, _) => 1
    | ((-1), _) => (-1)
    | (0, 1) => 1
    | (0, (-1)) => (-1)
    | (0, 0) => 0
    | (_, _) =>
      raise(BadComparison("comparison switch got an impossible value!"))
    };
  };

  let adjacentDiff: list(coords) = [
    (1, 1),
    (1, 0),
    (1, (-1)),
    (0, 1),
    (0, (-1)),
    ((-1), 1),
    ((-1), 0),
    ((-1), (-1)),
  ];

  let getAdjacent = ((x, y): coords, (xSize, ySize): size) => {
    adjacentDiff
    |> List.map(((xDiff, yDiff)) => (x + xDiff, y + yDiff))
    // filter out of bounds
    |> List.filter(((x, y)) => x >= 0 && x < xSize && y >= 0 && y < ySize);
  };

  type getNumAdjacent = (coords, list(coords)) => int;
  let getNumAdjacent: getNumAdjacent =
    ((x, y), countedCoords) =>
      adjacentDiff
      |> List.map(((xDiff, yDiff)) => (x + xDiff, y + yDiff))
      |> List.filter(coords =>
           List.exists(checked => checked == coords, countedCoords)
         )
      |> List.length;
};

module CoordsSet = Set.Make(Coords);
module CoordsMap = Map.Make(Coords);
module MyList = {
  let combinationRange = (a: int, b: int): list(coords) => {
    Belt.List.makeBy(a, i => Belt.List.makeBy(b, j => (j, i)))->List.concat;
  };
} /* }*/ /* module Ratio = */ /*   let greaterThan = ((an, ad), (bn, bd)) => float_of_in*/ /*   let min = (list: list(t)) => List.fold_left((a, b) => float_of_int(a)  > float_of_int(b) ? a : b)*/ /*   let max = (list: list(t)) => List.fold_left((a, b) => float_of_int()  < float_of_int(b) ? a : b, (0 , 1), list)*/ /*   type t = (int, int); // num, deno*/;