open Ctypes;
open Structs;
open Bindings;
open Utils;

module Publish = {
  let toggleOn = (message, hermes) => {
    hermes_sound_feedback_publish_toggle_on(
      hermes |> Hermes.sound,
      allocate(CSiteMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let toggleOff = (message, hermes) => {
    hermes_sound_feedback_publish_toggle_off(
      hermes |> Hermes.sound,
      allocate(CSiteMessage.view, message),
    )
    |> checkSnipsResult;
  };
};