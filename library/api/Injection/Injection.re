open Ctypes;
open Structs;
open Bindings;
open Utils;

module Publish = {
  let injectionRequest = (message, hermes) => {
    hermes_injection_publish_injection_request(
      hermes |> Hermes.injection,
      allocate(CInjectionRequestMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let injectionResetRequest = (message, hermes) => {
    hermes_injection_publish_injection_reset_request(
      hermes |> Hermes.injection,
      allocate(CInjectionResetRequestMessage.view, message),
    )
    |> checkSnipsResult;
  };
  let injectionStatusRequest = hermes => {
    hermes_injection_publish_injection_status_request(
      hermes |> Hermes.injection,
    )
    |> checkSnipsResult;
  };
};

module Subscribe = {
  let injectionComplete = (~callback, ~once=false, hermes) => {
    `SubscribeInjectionComplete((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let injectionResetComplete = (~callback, ~once=false, hermes) => {
    `SubscribeInjectionResetComplete((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let injectionStatus = (~callback, ~once=false, hermes) => {
    `SubscribeInjectionStatus((callback, once))
    |> Hermes.perform(hermes)
    |> ignore;
  };
};

module Unsubscribe = {
  let injectionComplete = (~callback, hermes) => {
    `UnubscribeInjectionComplete(callback)
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let injectionResetComplete = (~callback, hermes) => {
    `UnsubscribeInjectionResetComplete(callback)
    |> Hermes.perform(hermes)
    |> ignore;
  };
  let injectionStatus = (~callback, hermes) => {
    `UnsubscribeInjectionStatus(callback) |> Hermes.perform(hermes) |> ignore;
  };
};