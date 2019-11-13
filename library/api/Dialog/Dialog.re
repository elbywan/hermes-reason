open Ctypes;
open Structs;
open Bindings;
open Utils;

module Publish = {
  let startSession = (message, hermes) => {
    hermes_dialogue_publish_start_session(
      hermes |> Hermes.dialogue,
      allocate(CStartSessionMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let continueSession = (message, hermes) => {
    hermes_dialogue_publish_continue_session(
      hermes |> Hermes.dialogue,
      allocate(CContinueSessionMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let endSession = (message, hermes) => {
    hermes_dialogue_publish_end_session(
      hermes |> Hermes.dialogue,
      allocate(CEndSessionMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let configure = (message, hermes) => {
    hermes_dialogue_publish_configure(
      hermes |> Hermes.dialogue,
      allocate(CDialogueConfigureMessage.view, message),
    )
    |> checkSnipsResult;
  };
};

module Subscribe = {
  let intent = (~callback, ~intent, ~once=false, hermes) => {
    `SubscribeIntent((intent, callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let intents = (~callback, ~once=false, hermes) => {
    `SubscribeIntents((callback, once)) |> Hermes.perform(hermes) |> ignore;
  };
  let intentNotRecognized = (~callback, ~once=false, hermes) => {
    `SubscribeIntentNotRecognized((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let sessionStarted = (~callback, ~once=false, hermes) => {
    `SubscribeSessionStarted((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let sessionQueued = (~callback, ~once=false, hermes) => {
    `SubscribeSessionQueued((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let sessionEnded = (~callback, ~once=false, hermes) => {
    `SubscribeSessionEnded((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
};

module Unsubscribe = {
  let intent = (~callback, ~intent, hermes) => {
    `UnsubscribeIntent((intent, callback))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let intents = (~callback, hermes) => {
    `UnsubscribeIntents(callback) |> Hermes.perform(hermes) |> ignore;
  };
  let intentNotRecognized = (~callback, hermes) => {
    `UnsubscribeIntentNotRecognized(callback)
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let sessionStarted = (~callback, hermes) => {
    `UnsubscribeSessionStarted(callback) |> Hermes.perform(hermes) |> ignore;
  };
  let sessionQueued = (~callback, hermes) => {
    `UnsubscribeSessionQueued(callback) |> Hermes.perform(hermes) |> ignore;
  };
  let sessionEnded = (~callback, hermes) => {
    `UnsubscribeSessionEnded(callback) |> Hermes.perform(hermes) |> ignore;
  };
};