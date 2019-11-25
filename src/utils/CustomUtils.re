open GlobalTypes;

module Matrix = {
  let map = (func: ('a, coords) => 'b, matrix: matrix('a)) =>
    matrix
    |> Array.mapi((i, row) =>
         Array.mapi((j, e: 'a) => func(e, (j, i)), row)
       );
};