open GlobalTypes;

module Matrix = {
  let map = (func: ('a, coords) => 'b, matrix: matrix('a)) =>
    matrix
    |> Array.mapi((i, row) =>
         Array.mapi((j, e: 'a) => func(e, (j, i)), row)
       );
  let size = (matrix: matrix('a)) =>
    if (Array.length(matrix) == 0 || Array.length(matrix[0]) == 0) {
      (0, 0);
    } else {
      (Array.length(matrix[0]), Array.length(matrix));
    };
};