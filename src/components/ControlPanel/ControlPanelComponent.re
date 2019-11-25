open GlobalTypes;

[@react.component]
let make = (~onNewGame: unit => unit) => {
  <section>
    <button onClick={_ => onNewGame()}> {str("new game")} </button>
  </section>;
};