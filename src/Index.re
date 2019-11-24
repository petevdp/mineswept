// Entry point;
[@bs.val] external document: Js.t({..}) = "document";
// We're using raw DOM manipulations here, to avoid making you read
// ReasonReact when you might precisely be trying to learn it for the first
// time through the examples later.
let style = document##createElement("style");
document##head##appendChild(style);
style##innerHTML #= ExampleStyles.style;

let makeContainer = text => {
  let container = document##createElement("div");
  container##className #= "container";

  let title = document##createElement("div");
  title##className #= "containerTitle";
  title##innerText #= text;

  let content = document##createElement("div");
  content##className #= "containerContent";

  let () = container##appendChild(title);
  let () = container##appendChild(content);
  let () = document##body##appendChild(container);

  content;
};

// ReactDOMRe.render(
//   <BlinkingGreeting> {React.string("Hello!")} </BlinkingGreeting>,
//   makeContainer("Blinking Greeting"),
// );
// let matrix: Board.cellModelMatrix = [|[|{state: Cell.Visible}|]|];
let visibleCell: Cell.model = {state: Cell.Visible, mined: true};
let matrix: Board.cellModelMatrix = Array.make_matrix(10, 10, visibleCell);
let testMatrix: Board.cellModelMatrix = [|
  [|
    {state: Cell.Hidden, mined: false},
    {state: Cell.Visible, mined: true},
    {state: Cell.Visible, mined: false},
    {state: Cell.Flagged, mined: false},
  |],
|];

ReactDOMRe.render(
  <Board cellModelMatrix=testMatrix />,
  makeContainer("Cells"),
);

ReactDOMRe.render(
  <Board cellModelMatrix=matrix />,
  makeContainer("minesweeper"),
);