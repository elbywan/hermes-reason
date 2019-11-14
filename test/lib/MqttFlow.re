open HermesReason;

include MqttPrelude;

let sendAndReceive = (~timeout=0.2, ~message, ~sendTopic, ~receiveTopic, ()) => {
  open Tools;
  let port = port^;

  let sendIntentMessage = () =>
    publishMessage(~port, sendTopic, message) |> ignore;

  Unix.sleepf(timeout);

  receiveMessage(
    ~port,
    ~callback=sendIntentMessage,
    ~timeout=0.5,
    receiveTopic,
  );
};

let endFlow = () => {
  open Tools;
  let message = readMessage("./test/lib/messages/SessionEnded.json");
  publishMessage(~port=port^, "hermes/dialogueManager/sessionEnded", message)
  |> ignore;
  Unix.sleepf(0.2);
};

describe("Dialog flow api", ({test, _}) => {
  open Tools;

  test("Single dialog flow round", ({expect, env: hermes}) => {
    open Flow;

    let intentMessage = readMessage("./test/lib/messages/Intent.json");

    hermes
    |> entry(~intent="test1", ~callback=(msg, flow) => {
         expect.equal(Messages.intentMessage, msg);
         flow |> speak("Session ended");
       });

    let receivedMessage =
      sendAndReceive(
        ~message=intentMessage,
        ~sendTopic="hermes/intent/test1",
        ~receiveTopic="hermes/dialogueManager/endSession",
        (),
      );
    expect.string(receivedMessage).toMatchSnapshot();
    endFlow();
  });

  test("Single round with not recognized message", ({expect, env: hermes}) => {
    open Flow;

    let intentMessage = readMessage("./test/lib/messages/Intent.json");
    let notRecognizedMessage =
      readMessage("./test/lib/messages/IntentNotRecognized.json");

    let callback = (msg, flow) => {
      expect.equal(Messages.intentMessage, msg);
      flow
      |> continue(~intent="intentA", ~callback=(_, f) => f)
      |> continue(~intent="intentB", ~callback=(_, f) => f)
      |> notRecognized(~callback=(msg, flow) => {
           expect.equal(Messages.intentNotRecognized, msg);
           flow |> speak("Session ended");
         })
      |> speak("Text");
    };

    hermes |> entry(~intent="test2", ~callback);

    let receivedMessage =
      sendAndReceive(
        ~message=intentMessage,
        ~sendTopic="hermes/intent/test2",
        ~receiveTopic="hermes/dialogueManager/continueSession",
        (),
      );
    expect.string(receivedMessage).toMatchSnapshot();

    Unix.sleepf(0.5);

    let receivedMessage =
      sendAndReceive(
        ~message=notRecognizedMessage,
        ~sendTopic="hermes/dialogueManager/intentNotRecognized",
        ~receiveTopic="hermes/dialogueManager/endSession",
        (),
      );
    expect.string(receivedMessage).toMatchSnapshot();
    endFlow();
  });

  test("Multiple dialog flow rounds", ({expect, env: hermes}) => {
    open Flow;

    let maxRounds = 10;
    let rounds = ref(0);

    let intentMessage = readMessage("./test/lib/messages/Intent.json");
    let rec roundCallback = (_, flow) => {
      let continueSession = rounds^ < maxRounds;
      rounds := rounds^ + 1;
      if (continueSession) {
        flow
        |> continue(
             ~intent="test" ++ string_of_int(rounds^ + 3),
             ~callback=roundCallback,
           )
        |> speak("Session continued");
      } else {
        flow |> speak("Session ended");
      };
    };

    hermes |> entry(~intent="test3", ~callback=roundCallback);

    for (x in 0 to maxRounds) {
      let receiveTopic =
        x === maxRounds
          ? "hermes/dialogueManager/endSession"
          : "hermes/dialogueManager/continueSession";
      let sendTopic = "hermes/intent/test" ++ string_of_int(x + 3);
      let receivedMessage =
        sendAndReceive(~message=intentMessage, ~sendTopic, ~receiveTopic, ());
      expect.string(receivedMessage).not.toBeEmpty();
      expect.string(receivedMessage).toMatchSnapshot();
    };

    endFlow();
  });
});