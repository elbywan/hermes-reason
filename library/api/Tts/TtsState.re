open Ctypes;
open Structs;
open Bindings;

type t = {facade: lazy_t(ptr(CTtsFacade.t_view))};

let make = protocolHandler => {
  facade:
    lazy(
      StateUtils.getFacade(
        protocolHandler,
        CTtsFacade.view,
        hermes_protocol_handler_tts_facade,
      )
    ),
};