open HermesReason;
open Structs;

let run = () => {
  Unix.putenv("RUST_LOG", "debug");

  let hermes =
    Hermes.make(
      ~libraryPath="../hermes-protocol/target/debug/libhermes_mqtt_ffi",
      ~debug=false,
      (),
    );

  hermes
  |> Dialog.Publish.startSession(
       CStartSessionMessage.{
         site_id: "default",
         init: Notification("Hello"),
         custom_data: None,
       },
     );

  let callback = _ => {
    /* Console.log(intentMessage); */
    Console.log(
      "Should be off 5 sec after start",
    );
  };

  hermes |> Dialog.Subscribe.intents(~callback);
  hermes
  |> Dialog.Subscribe.intent(~intent="testIntent", ~callback=_ => {
       Console.log("Early listener")
     });
  hermes
  |> Dialog.Subscribe.intents(~once=true, ~callback=_ => {
       Console.log("Should trigger only once and for any intent")
     });

  Unix.sleep(5);

  Console.log("Plug off handler…");

  hermes |> Dialog.Unsubscribe.intents(~callback);
  hermes
  |> Dialog.Subscribe.intents(~callback=_ => {
       /* Console.log(intentMessage) */
       Console.log("Late listener")
     });

  /*
   let wav_bytes_list = [0, 1, 2, 3];
   let wav_sound = wav_bytes_list |> List.map(Unsigned.UInt8.of_int);
   let message: RegisterSoundMessage.t_view = {
     sound_id: "sound:id",
     wav_sound,
   };

   hermes |> Tts.Publish.registerSound(message); */

  while (true) {
    Unix.sleep(60);
  };
};