open Ctypes;
open Structs;
open Bindings;
open Utils;

module Publish = {
  let registerSound = (message, hermes) => {
    hermes_tts_publish_register_sound(
      hermes |> Hermes.tts,
      allocate(RegisterSoundMessage.view, message),
    )
    |> checkSnipsResult;
  };
};