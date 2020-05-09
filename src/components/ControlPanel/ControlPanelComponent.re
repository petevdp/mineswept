open CustomUtils;

type handlers = {
  onRewindGame: int => unit,
  onMakeEngineMove: unit => unit,
  onPlayGameWithEngine: unit => unit,
  onToggleOverlay: unit => unit,
};

[@react.component]
let make =
    (
      ~onNewGame: unit => unit,
      ~gamePhase: GameModel.phase,
      ~minesLeft: int,
      ~handlers: handlers,
    ) => {
  let gamePhaseMsg =
    switch (gamePhase) {
    | Start => "new"
    | Playing => "in progress"
    | Ended(Win) => "You Win"
    | Ended(Loss) => "You lose"
    };

  let {onRewindGame, onMakeEngineMove, onPlayGameWithEngine, onToggleOverlay} = handlers;
  <section>
    <button onClick={_ => onNewGame()}> {str("new game")} </button>
    <button onClick={_ => onRewindGame(1)}> {str("rewind game")} </button>
    <button onClick={_ => onMakeEngineMove()}>
      {str("Make Engine Move")}
    </button>
    <button onClick={_ => onPlayGameWithEngine()}>
      {str("Play game out with engine")}
    </button>
    <button onClick={_ => onToggleOverlay()}>
      {str("Toggle Overlay")}
    </button>
    <span> {str(gamePhaseMsg)} </span>
    <span> {str("  mines left: " ++ string_of_int(minesLeft))} </span>
  </section>;
};