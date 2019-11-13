let findOpenPort = () => {
  open Unix;

  let sock = socket(~cloexec=true, PF_INET, SOCK_STREAM, 0);
  bind(sock, ADDR_INET(inet_addr_any, 0));
  let port =
    switch (getsockname(sock)) {
    | ADDR_INET(_, port) => port
    | _ => 0
    };
  close(sock);
  port;
};

let launchMosquitto = (~silent=true, port) => {
  let port = string_of_int(port);
  let devNull = Unix.openfile("/dev/null", [Unix.O_WRONLY], 0x640);
  Console.log("\n> Launching mosquitto [PORT:" ++ port ++ "]");
  Unix.create_process(
    "mosquitto",
    [|"mosquitto", "-p", port, "-v"|],
    Unix.stdin,
    silent ? devNull : Unix.stdout,
    silent ? devNull : Unix.stderr,
  );
};

let launchMosquittoConfiguration = (~silent=true, configuration) => {
  let devNull = Unix.openfile("/dev/null", [Unix.O_WRONLY], 0x640);
  Console.log(
    "\n> Launching mosquitto using a configuration file. [PATH:"
    ++ configuration
    ++ "]",
  );
  Unix.create_process(
    "mosquitto",
    [|"mosquitto", "-c", configuration, "-v"|],
    Unix.stdin,
    silent ? devNull : Unix.stdout,
    silent ? devNull : Unix.stderr,
  );
};

let killMosquitto = pid =>
  if (pid > 0) {
    Console.log(
      "> Killing spawned mosquitto process [PID:" ++ string_of_int(pid) ++ "]",
    );
    Unix.kill(pid, Sys.sigkill);
    Unix.waitpid([WNOHANG], pid) |> ignore;
  };

let publishMessage = (~port, topic, message) => {
  Unix.create_process(
    "mosquitto_pub",
    [|
      "mosquitto_pub",
      "-p",
      string_of_int(port),
      "-t",
      topic,
      "-m",
      message,
    |],
    Unix.stdin,
    Unix.stdout,
    Unix.stderr,
  );
};

let receiveMessage = (~timeout=0.2, ~callback=() => (), ~port, topic) => {
  let message = ref("");

  try({
    let channel =
      Unix.open_process_in(
        "mosquitto_sub -p " ++ string_of_int(port) ++ " -t " ++ topic,
      );

    Unix.sleepf(timeout);

    callback();

    let thread =
      Thread.create(
        () =>
          try(
            while (true) {
              message :=
                String.concat(
                  "",
                  [message^, Char.escaped @@ input_char(channel)],
                );
            }
          ) {
          | _ => close_in_noerr(channel)
          },
        (),
      );

    Unix.sleepf(timeout);

    Unix.kill(Unix.process_in_pid(channel), Sys.sigkill);
    Thread.join(thread);
  }) {
  | e =>
    Printexc.print_backtrace(stderr);
    raise(e);
  };

  message^;
};

let readMessage = file => {
  let channel = open_in(file);
  let message = ref("");

  try(
    while (true) {
      message :=
        String.concat("", [message^, Char.escaped @@ input_char(channel)]);
    }
  ) {
  | _ => close_in_noerr(channel)
  };
  message^ |> Str.(global_replace(regexp("\\\\n"), ""));
};