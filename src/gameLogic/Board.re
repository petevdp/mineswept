open GlobalTypes;

type matrix('t) = array(array('t));
type model = matrix(Cell.model);

let adjacentDiff: list(coords) = [
  (1, 1),
  (1, 0),
  (1, (-1)),
  (0, 1),
  (0, (-1)),
  ((-1), 1),
  ((-1), 0),
  ((-1), (-1)),
] /* */;

// let getAdjacentCells = ((x, y): coords, matrix: matrix) => {
//   let xSize = Array.length(matrix[0]);
//   let ySize = Array.length(matrix);

//   let adjCoords = adjacentDiff
//   |> List.map((xDiff, yDiff) => )