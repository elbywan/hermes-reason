/**
  {1 Overview}

  The Hermes module is the entry point to hermes-reason.
  It can be used to connect and communicate with the Snips platform.
*/;

open Bindings;
open Structs;
open Utils;
open Ctypes;

/**
  The Hermes instance state.
*/
type t = {
  protocolHandler: ptr(CProtocolHandler.t_view),
  state: State.t,
};

/**
 Default Hermes options: no encryption and a local address on port 1883.
*/
let defaultOptions =
  CMqttOptions.{
    broker_address: "localhost:1883",
    username: None,
    password: None,
    tls_hostname: None,
    tls_ca_file: None,
    tls_ca_path: None,
    tls_client_key: None,
    tls_client_cert: None,
    tls_disable_root_store: false,
  };

/**
  Create a new Hermes state by connecting to the broker.
*/
let make =
    (
      ~libraryPath="./libhermes_mqtt_ffi",
      ~debug=false,
      ~options=defaultOptions,
      (),
    )
    : t => {
  let opened = Utils.openDynamicLibrary(libraryPath);

  if (opened == false) {
    raise(
      Failure("Could not open the hermes_mqtt_ffi dynamic library file."),
    );
  };

  if (debug) {
    hermes_enable_debug_logs() |> checkSnipsResult;
  };

  let protocolHandlerDblPtr =
    allocate(
      ptr @@ CProtocolHandler.view,
      from_voidp(CProtocolHandler.view, null),
    );

  let mqttOptions = allocate(CMqttOptions.view, options);

  hermes_protocol_handler_new_mqtt_with_options(
    protocolHandlerDblPtr,
    mqttOptions,
    null,
  )
  |> checkSnipsResult;

  let protocolHandler = !@protocolHandlerDblPtr;

  {protocolHandler, state: State.make(protocolHandler)};
};

/* Accessors */

let dialogue = hermes => Lazy.force @@ hermes.state.dialog.facade;
let injection = hermes => Lazy.force @@ hermes.state.injection.facade;
let sound = hermes => Lazy.force @@ hermes.state.soundFeedback.facade;
let tts = hermes => Lazy.force @@ hermes.state.tts.facade;
let flowSessions = hermes => hermes.state.dialog.flowSessions^;

let perform = (hermes, action) => {
  ...hermes,
  state: State.reduce(hermes.state, action),
};

/* Destructor */

/**
  Destroy the hermes instance and free underlying resources.
*/
let destroy = (hermes: t) => {
  let destroyFacade = (facade, destroy) =>
    if (Lazy.is_val @@ facade) {
      destroy(Lazy.force @@ facade) |> checkSnipsResult;
    };

  destroyFacade(hermes.state.dialog.facade, hermes_drop_dialogue_facade);
  destroyFacade(hermes.state.injection.facade, hermes_drop_injection_facade);
  destroyFacade(
    hermes.state.soundFeedback.facade,
    hermes_drop_sound_feedback_facade,
  );
  destroyFacade(hermes.state.tts.facade, hermes_drop_tts_facade);

  hermes_destroy_mqtt_protocol_handler(hermes.protocolHandler)
  |> checkSnipsResult;
};