open TestFramework;
open HermesReason;
open Tools;

/* Hermes debug mode. */
Unix.putenv("RUST_LOG", "debug");
let debug = false;
/* Mosquitto verbosity. */
let silent = true;

/* Lifecycles. */
let {describe, describeSkip, describeOnly} =
  describeConfig
  |> withLifecycle(testLifecycle => {
       testLifecycle
       |> beforeAll(() => {
            let pid =
              launchMosquittoConfiguration(
                ~silent,
                "./test/lib/tls/mosquitto-tls.conf",
              );
            /* Give some time to mosquitto… */
            Unix.sleepf(0.5);
            try({
              let hermes =
                Hermes.make(
                  ~libraryPath="./hermes-protocol/target/debug/libhermes_mqtt_ffi",
                  ~debug,
                  ~options={
                    ...Hermes.defaultOptions,
                    broker_address: "localhost:18886",
                    username: Some("foo"),
                    password: Some("bar"),
                    tls_hostname: Some("localhost"),
                    tls_ca_file: Some(["./test/lib/tls/ca.cert"]),
                    tls_client_key: Some("./test/lib/tls/client.key"),
                    tls_client_cert: Some("./test/lib/tls/client.cert"),
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
       |> afterAll(((mosquittoPid, hermes)) => {
            Hermes.destroy(hermes);
            killMosquitto(mosquittoPid);
          })
       |> beforeEach(((_, hermes)) => hermes)
     })
  |> build;

describe("Secured mosquitto connection", ({test, _}) => {
  test("Dummy message", _ => {
    ()
  })
});