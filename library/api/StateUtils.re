open Ctypes;
open Utils;

let flattenOptList =
  fun
  | None => []
  | Some(l) => l;

let getFacade = (protocolHandler, view, binding) => {
  let facadeDblPtr = allocate(ptr(view), from_voidp(view, null));
  binding(protocolHandler, facadeDblPtr) |> checkSnipsResult;
  !@facadeDblPtr;
};

module StringMap =
  Map.Make({
    type t = string;
    let compare = String.compare;
  });

module SubscriptionList = (Message: {type t;}) => {
  type subscription = [
    | `Subscription(Message.t => unit)
    | `SingleSubscription(Message.t => unit)
  ];

  type t = option(list(subscription));
  type t_no_opt = list(subscription);
};

let unsubscribe = (~subscription, ~getSubscriptions, ~setSubscriptions) => {
  getSubscriptions()
  |> Option.map(
       List.filter(
         fun
         | `Subscription(cb)
         | `SingleSubscription(cb) => cb !== subscription,
       ),
     )
  |> setSubscriptions;
};

let subscribe =
    (
      ~once=false,
      ~subscription,
      ~getSubscriptions,
      ~setSubscriptions,
      ~hermesFun,
      ~lazyFacade,
    ) => {
  let element =
    once ? `SingleSubscription(subscription) : `Subscription(subscription);
  getSubscriptions()
  |> (
    fun
    | Some(list) => Some([element, ...list])
    | None => {
        hermesFun(Lazy.force @@ lazyFacade, msg => {
          getSubscriptions()
          |> flattenOptList
          |> List.iter(
               fun
               | `Subscription(cb) => cb(msg)
               | `SingleSubscription(cb) => {
                   cb(msg);
                   unsubscribe(
                     ~subscription=cb,
                     ~getSubscriptions,
                     ~setSubscriptions,
                   );
                 },
             )
        })
        |> checkSnipsResult;
        Some([element]);
      }
  )
  |> setSubscriptions;
};