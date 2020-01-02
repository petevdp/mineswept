type props = {data: list(Js.Dict.key)};

[@react.component]
let make = (~data, ~children) => {
  let child = children;
  ReasonReact.cloneElement(
    child,
    ~props=Obj.magic(Js.Dict.fromList(data)),
    [||],
  );
};