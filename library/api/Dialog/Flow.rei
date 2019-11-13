type t
and intentListener = (Structs.CIntentMessage.t_view, t) => t
and notRecognizedListener =
  (Structs.CIntentNotRecognizedMessage.t_view, t) => t;

let entry:
  (
    ~intent: StateUtils.StringMap.key,
    ~callback: (Structs.CIntentMessage.t_view, t) => t,
    Hermes.t
  ) =>
  unit;

let continue: (~intent: string, ~callback: intentListener, t) => t;
let slotFiller:
  (~intent: string, ~callback: intentListener, ~slot: string, t) => t;
let notRecognized: (~callback: notRecognizedListener, t) => t;
let speak: (string, t) => t;