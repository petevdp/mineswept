open CustomUtils;

[@react.component]
let make = () => {
  <div>
    <DataAttributesProvider data=[("data-tip", "hello world")]>
      <span> {str("hi")} </span>
    </DataAttributesProvider>
    <ReactToolTip />
  </div>;
};