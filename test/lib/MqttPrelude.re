open TestFramework;
open HermesReason;
open Tools;

/* Hermes debug mode. */
Unix.putenv("RUST_LOG", "debug");
let debug = false;
/* Mosquitto verbosity. */
let silent = true;

/* Mosquitto port. */
let port = ref(0);

/* Generic test about publishing mqtt messages. */
let testPublish = (~expect, ~hermes, ~topic, callback) =>
  try(
    expect.string(
      receiveMessage(~port=port^, ~callback=() => callback(hermes), topic),
    ).
      toMatchSnapshot()
  ) {
  | e =>
    Printexc.print_backtrace(stderr);
    raise(e);
  };

/* Generic test about subscribing to mqtt events. */
let testSubscribe = (~expect, ~hermes, ~topic, ~name, subscription) =>
  try(
    {
      hermes
      |> subscription
      |> (_ => readMessage("./test/lib/messages/" ++ name ++ ".json"))
      |> publishMessage(~port=port^, topic)
      |> ignore;

      Unix.sleepf(0.2);

      expect.assertions(1);
    }
  ) {
  | e =>
    Printexc.print_backtrace(stderr);
    raise(e);
  };

/* Lifecycles. */
let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle => {
       testLifecycle
       |> beforeAll(() => {
            port := findOpenPort();
            let pid = launchMosquitto(~silent, port^);
            /* Give some time to mosquitto… */
            Unix.sleepf(0.5);
            try({
              let hermes =
                Hermes.make(
                  ~libraryPath=
                    "./hermes-protocol/target/debug/libhermes_mqtt_ffi",
                  ~debug,
                  ~options={
                    ...Hermes.defaultOptions,
                    broker_address: "localhost:" ++ string_of_int(port^),
                  },
                  (),
                );
              (pid, hermes);
            }) {
            | fail =>
              killMosquitto(pid);
              raise(fail);
            };
          })
       |> afterAll(((mosquittoPid, hermes)) =>
            try(
              {
                Hermes.destroy(hermes);
                killMosquitto(mosquittoPid);
                Unix.sleepf(0.5);
              }
            ) {
            | e =>
              Printexc.print_backtrace(stderr);
              raise(e);
            }
          )
       |> beforeEach(((_, hermes)) => hermes)
     })
  |> build;