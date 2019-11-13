open HermesReason;

let run = () => {
  Unix.putenv("RUST_LOG", "debug");

  let hermes =
    Hermes.make(
      ~libraryPath="../hermes-protocol/target/debug/libhermes_mqtt_ffi",
      ~debug=true,
      (),
    );

  hermes
  |> Flow.entry(~intent="some:intent", ~callback=(msg, flow) => {
       open Flow;

       let byeAndEnd = (_, flow) => {
         flow |> speak("Bye!");
       };

       flow
       |> continue(~intent="some:farewell", ~callback=byeAndEnd)
       |> notRecognized(~callback=(_, flow) => {
            flow
            |> continue(~intent="some:intent", ~callback=byeAndEnd)
            |> speak(
                 "I'm waiting for you to say something to end the flow…",
               )
          })
       |> speak(
            "You just said :" ++ msg.input ++ ". Now please end the flow.",
          );
     });

  while (true) {
    Unix.sleep(60);
  };
};