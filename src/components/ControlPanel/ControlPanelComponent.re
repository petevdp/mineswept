open GlobalTypes;

[@react.component]
let make = (~onNewGame: unit => unit, ~gamePhase: Game.phase) => {
  let gamePhaseMsg =
    switch (gamePhase) {
    | Start => "new"
    | Playing => "in progress"
    | Ended(Win) => "You Win"
    | Ended(Loss) => "You lose"
    };
  <section>
    <button onClick={_ => onNewGame()}> {str("new game")} </button>
    <span> {str(gamePhaseMsg)} </span>
  </section>;
};