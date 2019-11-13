open Ctypes;
open Structs;
open Bindings;
open StateUtils;

module InjectionCompleteSubscriptions =
  SubscriptionList({
    type t = CInjectionCompleteMessage.t_view;
  });
module InjectionResetCompleteSubscriptions =
  SubscriptionList({
    type t = CInjectionResetCompleteMessage.t_view;
  });
module InjectionStatusSubscriptions =
  SubscriptionList({
    type t = CInjectionStatusMessage.t_view;
  });

type subscriptions = {
  mutable injectionComplete: InjectionCompleteSubscriptions.t,
  mutable injectionResetComplete: InjectionResetCompleteSubscriptions.t,
  mutable injectionStatus: InjectionStatusSubscriptions.t,
};

type t = {
  facade: lazy_t(ptr(CInjectionFacade.t_view)),
  subscriptions,
  handlers: ref(HList.t),
};

let make = protocolHandler => {
  facade:
    lazy(
      StateUtils.getFacade(
        protocolHandler,
        CInjectionFacade.view,
        hermes_protocol_handler_injection_facade,
      )
    ),
  subscriptions: {
    injectionComplete: None,
    injectionResetComplete: None,
    injectionStatus: None,
  },
  handlers: ref(HList.[]),
};

let reduce = state =>
  fun
  | `SubscribeInjectionComplete(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionComplete,
        ~setSubscriptions=s => state.subscriptions.injectionComplete = s,
        ~registerHandler=
          handler => state.handlers := HList.[handler, ...state.handlers^],
        ~hermesFun=hermes_injection_subscribe_injection_complete,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeInjectionComplete(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionComplete,
        ~setSubscriptions=s => state.subscriptions.injectionComplete = s,
      );
      state;
    }
  | `SubscribeInjectionResetComplete(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionResetComplete,
        ~setSubscriptions=s => state.subscriptions.injectionResetComplete = s,
        ~registerHandler=
          handler => state.handlers := HList.[handler, ...state.handlers^],
        ~hermesFun=hermes_injection_subscribe_injection_reset_complete,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeInjectionResetComplete(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionResetComplete,
        ~setSubscriptions=s => state.subscriptions.injectionResetComplete = s,
      );
      state;
    }
  | `SubscribeInjectionStatus(subscription, once) => {
      subscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionStatus,
        ~setSubscriptions=s => state.subscriptions.injectionStatus = s,
        ~registerHandler=
          handler => state.handlers := HList.[handler, ...state.handlers^],
        ~hermesFun=hermes_injection_subscribe_injection_status,
        ~lazyFacade=state.facade,
        ~once,
      );
      state;
    }
  | `UnsubscribeInjectionStatus(subscription) => {
      unsubscribe(
        ~subscription,
        ~getSubscriptions=() => state.subscriptions.injectionStatus,
        ~setSubscriptions=s => state.subscriptions.injectionStatus = s,
      );
      state;
    }
  | _ => state;