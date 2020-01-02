open GlobalTypes;

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

module Matrix = {
  let map = (~f: ('a, Coords.t) => 'b, matrix: matrix('a)) =>
    matrix
    |> Array.mapi((i, row) => Array.mapi((j, e: 'a) => f(e, (j, i)), row));

  let size = (matrix: matrix('a)) =>
    if (Array.length(matrix) == 0 || Array.length(matrix[0]) == 0) {
      (0, 0);
    } else {
      (Array.length(matrix[0]), Array.length(matrix));
    };

  let make = ((x, y): size, cell: 'a) => {
    Array.make_matrix(y, x, cell);
  };

  let flatten = Belt.Array.concatMany;

  let toList = (matrix: matrix('a)) => matrix->flatten->Array.to_list;

  let flattenWithCoords = (matrix: matrix('a)): array(('a, coords)) =>
    matrix
    |> map(~f=(cell, coords) => (cell, coords))
    |> Belt.Array.concatMany;

  let reduce =
      (~f: ('acc, 'cell, Coords.t) => 'acc, acc: 'acc, matrix: matrix('cell)) => {
    matrix
    |> flattenWithCoords
    |> Array.fold_left((acc, (cell, coords)) => f(acc, cell, coords), acc);
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
      ~f=(acc, a, _) => List.concat([acc, f(a) ? [a] : []]),
      [],
      matrix,
    );

  let getAdjacentWithCoords = (coords, t) => {
    Coords.getAdjacent(coords, size(t))
    |> List.map(coord => {
         let (x, y) = coord;
         (t[x][y], coord);
       });
  };
};

module IntUtils = {
  exception NoNumbersProvided;
  let max = (nums: list(int)) =>
    switch (nums) {
    | [num] => num
    | [num, ...rest] => List.fold_left((a, b) => a > b ? a : b, num, rest)
    | [] => raise(NoNumbersProvided)
    };

  let min = (nums: list(int)) =>
    switch (nums) {
    | [num] => num
    | [num, ...rest] => List.fold_left((a, b) => a < b ? a : b, num, rest)
    | [] => raise(NoNumbersProvided)
    };
};

// sets
module CoordsSet = Set.Make(Coords);

// maps
module CoordsMap = Map.Make(Coords);
module StrMap = Map.Make(String);

module MyList = {
  let combinationRange = (a: int, b: int): list(coords) => {
    Belt.List.makeBy(a, i => Belt.List.makeBy(b, j => (j, i)))->List.concat;
  };
};

let getRange = size => {
  let coordsSet = ref(CoordsSet.empty);
  let (xSize, ySize) = size;
  for (i in 0 to xSize) {
    for (j in 0 to ySize) {
      coordsSet := CoordsSet.add((i, j), coordsSet^);
    };
  };

  coordsSet;
};

let str = ReasonReact.string;