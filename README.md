# hermes-reason

#### A ReasonML wrapper around the hermes protocol

## Context

The `hermes-reason` library provides bindings for the [Hermes protocol](https://docs.snips.ai/reference/hermes) that [Snips](https://snips.ai/) components use to communicate together. `hermes-reason` allows you to interface seamlessly with the Snips platform and create Voice applications with ease!

`hermes-reason` abstracts away the connection to the MQTT bus and the parsing of incoming and outcoming messages from and to the components of the platform and provides a high-level API as well.

## Installation

`esy add hermes-reason`

## Usage

```reason
open HermesReason;
```

### Minimal use case

```reasonml
open HermesReason;

// Instantiate a Hermes object connecting to the default MQTT address (localhost:1883).
let hermes = Hermes.make();

// Declare a callback function
let callback = (msg: Structs.CIntentMessage.t_view, flow) => {
  open Flow;

  // End the session and speak.
  flow |> speak(
    "Received message for intent " ++ msg.intent.intent_name
  );
};

// Register the callback to trigger when intent 'myIntent' is detected.
hermes |> Flow.entry(
  ~intent="myIntent",
  ~callback
);

// To prevent the process from exiting, you can use sleep.
while (true) {
  Unix.sleep(60);
};
```

### Expanded use case

#### High-level `Flow` api.

```reasonml
open HermesReason;

let hermes = Hermes.make();

/*
  Using the high level API is strongly recommended for building complex dialog flows.

  The goal is to register the following dialog paths:

  A
  ├── B
  │   └─ D
  └── C

  In plain words, intent 'A' starts the flow, then restrain the next intents to 'B' or 'C'.
  If 'B' is the next intent detected, then next intent must be 'D' (and end the flow after 'D').
  If it was 'C', end the flow.
*/

hermes |> Flow.entry(~intent="A", ~callback=(msg, flow) => {
  open Flow;

  print_string("Intent A received. Session started.");

  /*
  At each step of the dialog flow, you have the choice of
  registering the next intents, or end the flow by not
  registering any continuations.

  We then subscribe to both intent B or C so that the dialog
  flow will continue with either one or the other next.
  */

  flow
  // Mark intent 'B' as one of the next dialog intents. (A -> B)
  |> continue(~intent="B", ~callback=(msg, flow) => {
    print_string("Intent B received. Session continued.");

    // Mark intent 'D'. (A -> B -> D)
    flow |> continue(~intent="D", ~callback=(_, flow) => {
        print_string("Intent D received. Session is ended.");
        flow |> speak("Finished the session with intent D.");
    });
  })
  // Mark intent 'C' as one of the next dialog intents. (A -> C)
  |> continue(~intent="C", ~callback=(msg, flow) => {
      open Structs;

      // Retrieve the first slot value, assuming it is a slot of type String.
      let Some(SlotValue.String(slotValue)) = msg.slots |> Option.map(slots => CSlot.((slots |> List.hd).value.value))
      print_string("Intent C received. Session is ended.");
      flow |> speak("Finished the session with intent C having value " ++ slotValue ++" .");
  })
  // Speak.
  |> speak("Continue with B or C.");
});
```

#### Low-level subscriber & publisher api.

```reasonml
open HermesReason;
open Structs;

let hermes = Hermes.make();

/*
  Every API facade can publish and receive data based on a list of events.

  For the purpose of this example, we will only use the Dialog facade, and the
  events related to a dialog session.

  Note that more events are available for each facade.
*/

// You can subscribe to an event triggered when the intent 'some_intent' is detected like this:
hermes
|> Dialog.Subscribe.intent(~intent="some_intent", ~callback=message => {

  // The 'message' argument contain all the data you need to perform an action based on what the user said.

  // For instance, you can grab a slot and its value like this.
  [@warning "-8"] // For the sake of brevity, we ignore the exhaustive pattern matching warning.
  let Some(someSlot) = message.slots |> Option.map(List.find(slot => CSlot.(slot.slot_name) === "some_slot"));
  [@warning "-8"]
  let SlotValue.String(slotValue) = someSlot.value.value;
  print_string("Slot value: " ++ slotValue);

  // And here is how to grab the intent name.
  let intentName = message.intent.intent_name;
  print_string("Received intent: " ++ intentName);

  let sessionShouldGoOn = true
  // Then, you can either (but not both):
  if (sessionShouldGoOn) {
    // Continue the current dialogue session.
    hermes |> Dialog.Publish.continueSession(CContinueSessionMessage.{
      // Session id is the same as the current session.
      session_id: message.session_id,
      // This is what is going to be spoken between this and the next session round.
      text: Some("The session lives on…"),
      // A list of possible intent continuations for the next round.
      intent_filter: Some(["next_intent"]),
      // Unused by the crystal wrapper.
      custom_data: None,
      // An optional slot filler argument. Not needed for this example.
      slot: None,
      // If true, then a custom behaviour can be determined if no intents are recognized for the next round of dialogue.
      send_intent_not_recognized: false,
    });
  } else {
    // End the dialogue session.
    hermes |> Dialog.Publish.endSession(CEndSessionMessage.{
      session_id: message.session_id,
      text: Some("The session has ended.")
    });
  }
});

// You can also unsubscribe to a registered event.
let rec callback = msg => {
  hermes |> Dialog.Unsubscribe.intents(~callback,);
};
hermes |> Dialog.Subscribe.intents(~callback);

// Or process a subscription only once:
hermes |> Dialog.Subscribe.intents(~callback, ~once=true);
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
