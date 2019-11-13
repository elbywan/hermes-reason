open Ctypes;
open Bindings;

let suffixes = [".so", ".dylib", ".exe", ""];

let openDynamicLibrary = (~flags=[Dl.RTLD_NOW], path) => {
  suffixes
  |> List.exists(suff => {
       let path = path ++ suff;
       try(
         {
           Dl.dlopen(~filename=path, ~flags) |> ignore;
           true;
         }
       ) {
       | _ => false
       };
     });
};

let checkSnipsResult = (~exn=true, snipsResult) =>
  switch (snipsResult) {
  | Error(_) =>
    let errorStringPtr = allocate(string, "");
    hermes_get_last_error(errorStringPtr) |> ignore;
    let errorMessage = !@errorStringPtr;
    if (exn) {
      raise(Failure(errorMessage));
    } else {
      Console.error(errorMessage);
    };
  | _ => ()
  };