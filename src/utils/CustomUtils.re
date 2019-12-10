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

  let flattenWithCoords = (matrix: matrix('a)): array(('a, coords)) =>
    matrix
    |> map(~f=(cell, coords) => (cell, coords))
    |> Belt.Array.concatMany;

  let reduce = (~acc: 'a, ~f: ('a, 'b) => 'a, matrix: matrix('b)): 'a => {
    let cells = flatten(matrix);
    Belt.Array.reduce(cells, acc, f);
  };

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

module MyList = {
  let combinationRange = (a: int, b: int): list(coords) => {
    Belt.List.makeBy(a, i => Belt.List.makeBy(b, j => (j, i)))->List.concat;
  };
};