open GlobalTypes;

[@react.component]
let make = (~onNewGame: unit => unit, ~gamePhase: Game.phase, ~minesLeft: int) => {
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
    <span> {str("  mines left: " ++ string_of_int(minesLeft))} </span>
  </section>;
};