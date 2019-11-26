open GlobalTypes;

[@react.component]
let make = (~onNewGame: unit => unit, ~gameState: Game.gameState) => {
  let gameStateMsg =
    switch (gameState) {
    | New => "new"
    | Playing => "in progress"
    | Ended => "game over"
    };
  <section>
    <button onClick={_ => onNewGame()}> {str("new game")} </button>
    <span> {str(gameStateMsg)} </span>
  </section>;
};