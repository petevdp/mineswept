open GlobalTypes;

[@react.component]
let make =
    (
      ~onNewGame: unit => unit,
      ~gamePhase: Game.phase,
      ~minesLeft: int,
      ~onRewindGame: int => unit,
      ~onMakeEngineMove: unit => unit,
    ) => {
  let gamePhaseMsg =
    switch (gamePhase) {
    | Start => "new"
    | Playing => "in progress"
    | Ended(Win) => "You Win"
    | Ended(Loss) => "You lose"
    };
  <section>
    <button onClick={_ => onNewGame()}> {str("new game")} </button>
    <button onClick={_ => onRewindGame(1)}> {str("rewind game")} </button>
    <button onClick={_ => onMakeEngineMove()}>
      {str("Make Engine Move")}
    </button>
    <span> {str(gamePhaseMsg)} </span>
    <span> {str("  mines left: " ++ string_of_int(minesLeft))} </span>
  </section>;
};