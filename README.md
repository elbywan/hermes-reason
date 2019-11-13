# hermes-reason

**Contains the following libraries and executables:**

```
hermes-reason@0.0.1
│
├─library/
│   library name: hermes-reason.lib
│   namespace:    HermesReason
│   require:      ctypes ctypes.foreign console.lib
│
├─test/lib/
│   library name: hermes-reason.test.lib
│   namespace:    HermesReasonTestLib
│   require:      hermes-reason.lib rely.lib ctypes ctypes.foreign
│
├─test/
│   name:    HermesReasonTests.exe
│   main:    HermesReasonTests
│   require: hermes-reason.test.lib
│
└─examples/
    name:    HermesReasonExample.exe
    main:    HermesReasonExample
    require: hermes-reason.lib ctypes ctypes.foreign
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x HermesReasonExample.exe
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
