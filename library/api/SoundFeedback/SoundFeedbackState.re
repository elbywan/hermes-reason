open Ctypes;
open Structs;
open Bindings;

type t = {facade: lazy_t(ptr(CSoundFeedbackFacade.t_view))};

let make = protocolHandler => {
    facade: lazy StateUtils.getFacade(
        protocolHandler,
        CSoundFeedbackFacade.view,
        hermes_protocol_handler_sound_feedback_facade
    )
}