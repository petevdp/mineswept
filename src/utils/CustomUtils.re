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
};

module MyList = {
  let combinationRange = (a: int, b: int): list(coords) => {
    Belt.List.makeBy(a, i => Belt.List.makeBy(b, j => (j, i)))->List.concat;
  };
};