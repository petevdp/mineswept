open CustomUtils;

type t = Board.restrictedModel => Game.action;

let getActionFromEngine = (~engine: t, board: Board.model) =>
  board |> Board.getRestrictedModel |> engine;

let firstAvailable: t = {
  board => {
    let (_, cellCoords) =
      board
      |> Matrix.flattenWithCoords
      |> Array.to_list
      |> List.filter(((cellState: Board.Cell.restrictedModel, _)) =>
           cellState == Hidden
         )
      |> List.hd;

    Game.Check(cellCoords);
  };
};