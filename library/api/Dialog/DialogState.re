open Ctypes;
open Structs;
open Bindings;
open StateUtils;

module IntentSubscriptions =
  SubscriptionList({
    type t = CIntentMessage.t_view;
  });
module IntentNotRecognizedSubscriptions =
  SubscriptionList({
    type t = CIntentNotRecognizedMessage.t_view;
  });
module SessionStartedSubscriptions =
  SubscriptionList({
    type t = CSessionStartedMessage.t_view;
  });
module SessionQueuedSubscriptions =
  SubscriptionList({
    type t = CSessionQueuedMessage.t_view;
  });
module SessionEndedSubscriptions =
  SubscriptionList({
    type t = CSessionEndedMessage.t_view;
  });

type subscriptions = {
  mutable intent: StringMap.t(IntentSubscriptions.t_no_opt),
  mutable intents: IntentSubscriptions.t,
  mutable notRecognized: IntentNotRecognizedSubscriptions.t,
  mutable sessionStarted: SessionStartedSubscriptions.t,
  mutable sessionQueued: SessionQueuedSubscriptions.t,
  mutable sessionEnded: SessionEndedSubscriptions.t,
};

type t = {
  facade: lazy_t(ptr(CDialogueFacade.t_view)),
  subscriptions,
  flowSessions: ref(list(string)),
};

let make = protocolHandler => {
  facade:
    lazy(
      getFacade(
        protocolHandler,
        CDialogueFacade.view,
        hermes_protocol_handler_dialogue_facade,
      )
    ),
  subscriptions: {
    intent: StringMap.empty,
    intents: None,
    notRecognized: None,
    sessionStarted: None,
    sessionQueued: None,
    sessionEnded: None,
  },
  flowSessions: ref([]),
};

let reduce = state =>
  fun
  | `SubscribeIntent(intent, subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=
          () => {state.subscriptions.intent |> StringMap.find_opt(intent)},
        ~setSubscriptions=
          subscriptions => {
            state.subscriptions.intent =
              state.subscriptions.intent
              |> StringMap.update(intent, _ => subscriptions)
          },
        ~hermesFun=
          (facade, cb) =>
            hermes_dialogue_subscribe_intent(facade, intent, cb),
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeIntent(intent, subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=
          () => {state.subscriptions.intent |> StringMap.find_opt(intent)},
        ~setSubscriptions=
          subscriptions => {
            state.subscriptions.intent =
              state.subscriptions.intent
              |> StringMap.update(intent, _ => subscriptions)
          },
      );
      state;
    }
  | `SubscribeIntents(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.intents,
        ~setSubscriptions=s => state.subscriptions.intents = s,
        ~hermesFun=hermes_dialogue_subscribe_intents,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeIntents(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.intents,
        ~setSubscriptions=s => state.subscriptions.intents = s,
      );
      state;
    }
  | `SubscribeIntentNotRecognized(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.notRecognized,
        ~setSubscriptions=s => state.subscriptions.notRecognized = s,
        ~hermesFun=hermes_dialogue_subscribe_intent_not_recognized,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeIntentNotRecognized(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.notRecognized,
        ~setSubscriptions=s => state.subscriptions.notRecognized = s,
      );
      state;
    }
  | `SubscribeSessionStarted(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionStarted,
        ~setSubscriptions=s => state.subscriptions.sessionStarted = s,
        ~hermesFun=hermes_dialogue_subscribe_session_started,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeSessionStarted(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionStarted,
        ~setSubscriptions=s => state.subscriptions.sessionStarted = s,
      );
      state;
    }
  | `SubscribeSessionQueued(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionQueued,
        ~setSubscriptions=s => state.subscriptions.sessionQueued = s,
        ~hermesFun=hermes_dialogue_subscribe_session_queued,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnubscribeSessionQueued(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionQueued,
        ~setSubscriptions=s => state.subscriptions.sessionQueued = s,
      );
      state;
    }
  | `SubscribeSessionEnded(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionEnded,
        ~setSubscriptions=s => state.subscriptions.sessionEnded = s,
        ~hermesFun=hermes_dialogue_subscribe_session_ended,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeSessionEnded(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.sessionEnded,
        ~setSubscriptions=s => state.subscriptions.sessionEnded = s,
      );
      state;
    }
  | `FlowStarted(sessionId) => {
      state.flowSessions := [sessionId, ...state.flowSessions^];
      state;
    }
  | `FlowEnded(sessionId) => {
      state.flowSessions :=
        state.flowSessions^ |> List.filter(id => id !== sessionId);
      state;
    }
  | _ => state;