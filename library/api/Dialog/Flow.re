open Structs;

type t = {
  hermes: Hermes.t,
  sessionId: string,
  continuations: list((string, intentListener)),
  notRecognized: option(notRecognizedListener),
  slotFiller: option(string),
  text: option(string),
  subscribers: ref(subscribers),
}
and intentListener = (CIntentMessage.t_view, t) => t
and notRecognizedListener = (CIntentNotRecognizedMessage.t_view, t) => t
and subscribers = {
  intents: list((string, CIntentMessage.t_view => unit)),
  nr: option(CIntentNotRecognizedMessage.t_view => unit),
};

let make = (hermes, sessionId) => {
  hermes,
  sessionId,
  continuations: [],
  notRecognized: None,
  slotFiller: None,
  text: None,
  subscribers: ref({intents: [], nr: None}),
};

let buildContinueMessage = flow => {
  let rec getIntentFilter = (~acc=Some([])) => {
    fun
    | [] => None
    | [(intent, _)] => acc |> Option.map(acc => [intent, ...acc])
    | [(intent, _), ...tail] =>
      getIntentFilter(~acc=acc |> Option.map(acc => [intent, ...acc]), tail);
  };

  CContinueSessionMessage.{
    session_id: flow.sessionId,
    intent_filter: getIntentFilter(flow.continuations),
    text: flow.text,
    custom_data: None,
    slot: flow.slotFiller,
    send_intent_not_recognized: flow.notRecognized |> Option.is_some,
  };
};

let buildEndSessionMessage = (flow: t) => {
  CEndSessionMessage.{session_id: flow.sessionId, text: flow.text};
};

let cleanUpRound = flow => {
  flow.subscribers^.intents
  |> List.iter(((intent, callback)) => {
       flow.hermes |> Dialog.Unsubscribe.intent(~intent, ~callback)
     });
  flow.subscribers^.nr
  |> Option.iter(callback => {
       flow.hermes |> Dialog.Unsubscribe.intentNotRecognized(~callback)
     });
};

let roundContinuation = (msg, cb, flow, done_) => {
  /* 1. Clean up previous listeners. */
  cleanUpRound(flow);

  /* Step 2. Create a flow and pass it to the user callback. */
  let flow = cb(msg, make(flow.hermes, flow.sessionId));

  /* Step 3. End the round. */
  done_(flow);
};

let rec endRound = flow =>
  /* If there are 0 continuations registered, then the flow is over. */
  if (flow.continuations |> List.length < 1) {
    /* Build an end session message and stop the session. */
    let message = flow |> buildEndSessionMessage;
    /* And publish the end session. */
    flow.hermes |> Dialog.Publish.endSession(message);
  } else {
    /* Otherwise, build the continue session message. */
    let message = flow |> buildContinueMessage;
    /* Set up the intent continuations. */
    flow.continuations
    |> List.iter(((intent, cb)) => {
         let callback = msg => {
           roundContinuation(msg, cb, flow, endRound);
         };
         flow.hermes |> Dialog.Subscribe.intent(~intent, ~callback);
         flow.subscribers :=
           {
             ...flow.subscribers^,
             intents: [(intent, callback), ...flow.subscribers^.intents],
           };
       });
    /* And the not recognized handler. */
    flow.notRecognized
    |> Option.iter(cb => {
         let callback = msg => {
           roundContinuation(msg, cb, flow, endRound);
         };
         flow.hermes |> Dialog.Subscribe.intentNotRecognized(~callback);
         flow.subscribers := {...flow.subscribers^, nr: Some(callback)};
       });
    /* And publish the continue session message. */
    flow.hermes |> Dialog.Publish.continueSession(message);
  };

let entry = (~intent, ~callback, hermes) => {
  hermes
  |> Dialog.Subscribe.intent(
       ~intent,
       ~callback=msg => {
         let sessionId = msg.session_id;

         let flowExists =
           hermes |> Hermes.flowSessions |> List.exists(id => id == sessionId);

         /* Guard. If the session id is already matching a running flow - do nothing. */
         if (!flowExists) {
           /* Step 1. Register the session id so that future intent calls do not trigger it twice. */
           Hermes.perform(hermes, `FlowStarted(sessionId)) |> ignore;

           /* Step 2. Add a session ended listener that will deregister this session when it ends. */
           let rec onSessionEnd = (msg: CSessionEndedMessage.t_view) =>
             if (msg.session_id == sessionId) {
               Hermes.perform(hermes, `FlowEnded(sessionId)) |> ignore;
               hermes
               |> Dialog.Unsubscribe.sessionEnded(~callback=onSessionEnd);
             };
           hermes |> Dialog.Subscribe.sessionEnded(~callback=onSessionEnd);

           /* Step 3. Create a flow and pass it to the user callback. */
           let flow = callback(msg, make(hermes, sessionId));

           /* Step 4. End the round. */
           endRound(flow);
         };
       },
     );
};

let continue = (~intent, ~callback, flow) => {
  ...flow,
  continuations: [(intent, callback), ...flow.continuations],
};
let slotFiller = (~intent, ~callback, ~slot, flow) => {
  ...flow,
  continuations: [(intent, callback), ...flow.continuations],
  slotFiller: Some(slot),
};
let notRecognized = (~callback, flow) => {
  ...flow,
  notRecognized: Some(callback),
};
let speak = (text, flow) => {...flow, text: Some(text)};