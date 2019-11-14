/**
  Contains all the enumerations related to the hermes library.
*/

/**
 The result of a call to the hermes library.
*/
let snips_result =
  Ctypes.(
    view(
      ~read=
        fun
        | 0 => Ok(0)
        | i => Error(i),
      ~write=
        fun
        | Ok(i) => i
        | Error(i) => i,
      int,
    )
  );

%enum
/**
  Enum representing the grain of a resolved date related value.
*/
type snips_grain =
  | SNIPS_GRAIN_YEAR
  | SNIPS_GRAIN_QUARTER
  | SNIPS_GRAIN_MONTH
  | SNIPS_GRAIN_WEEK
  | SNIPS_GRAIN_DAY
  | SNIPS_GRAIN_HOUR
  | SNIPS_GRAIN_MINUTE
  | SNIPS_GRAIN_SECOND;

%enum
/**
  Describes Snips components that are part of the platform.
*/
type snips_hermes_component =
  | [@as (-1)] SNIPS_HERMES_COMPONENT_NONE
  | [@as 1] SNIPS_HERMES_COMPONENT_AUDIO_SERVER
  | [@as 2] SNIPS_HERMES_COMPONENT_HOTWORD
  | [@as 3] SNIPS_HERMES_COMPONENT_ASR
  | [@as 4] SNIPS_HERMES_COMPONENT_NLU
  | [@as 5] SNIPS_HERMES_COMPONENT_DIALOGUE
  | [@as 6] SNIPS_HERMES_COMPONENT_TTS
  | [@as 7] SNIPS_HERMES_COMPONENT_INJECTION
  | [@as 8] SNIPS_HERMES_COMPONENT_CLIENT_APP;

%enum
/**
  Type of injection supported.
*/
type snips_injection_kind =
  | [@as 1] SNIPS_INJECTION_KIND_ADD
  | [@as 2] SNIPS_INJECTION_KIND_ADD_FROM_VANILLA;

%enum
/**
  Enum describing the precision of a resolved value.
*/
type snips_precision =
  | SNIPS_PRECISION_APPROXIMATE
  | SNIPS_PRECISION_EXACT;

%enum
/**
  The type of sessions that can be initiated programatically.
*/
type snips_session_init_type =
  | [@as 1] SNIPS_SESSION_INIT_TYPE_ACTION
  | [@as 2] SNIPS_SESSION_INIT_TYPE_NOTIFICATION;

%enum
/**
  The reasons that caused a session termination.
*/
type snips_session_termination_type =
  | [@as 1] SNIPS_SESSION_TERMINATION_TYPE_NOMINAL
  | [@as 2] SNIPS_SESSION_TERMINATION_TYPE_SITE_UNAVAILABLE
  | [@as 3] SNIPS_SESSION_TERMINATION_TYPE_ABORTED_BY_USER
  | [@as 4] SNIPS_SESSION_TERMINATION_TYPE_INTENT_NOT_RECOGNIZED
  | [@as 5] SNIPS_SESSION_TERMINATION_TYPE_TIMEOUT
  | [@as 6] SNIPS_SESSION_TERMINATION_TYPE_ERROR;

%enum
/**
  The type of slot values that are supported by Snips, either custom or built in.
*/
type snips_slot_value_type =
  | [@as 1] SNIPS_SLOT_VALUE_TYPE_CUSTOM
  | [@as 2] SNIPS_SLOT_VALUE_TYPE_NUMBER
  | [@as 3] SNIPS_SLOT_VALUE_TYPE_ORDINAL
  | [@as 4] SNIPS_SLOT_VALUE_TYPE_INSTANTTIME
  | [@as 5] SNIPS_SLOT_VALUE_TYPE_TIMEINTERVAL
  | [@as 6] SNIPS_SLOT_VALUE_TYPE_AMOUNTOFMONEY
  | [@as 7] SNIPS_SLOT_VALUE_TYPE_TEMPERATURE
  | [@as 8] SNIPS_SLOT_VALUE_TYPE_DURATION
  | [@as 9] SNIPS_SLOT_VALUE_TYPE_PERCENTAGE
  | [@as 10] SNIPS_SLOT_VALUE_TYPE_MUSICALBUM
  | [@as 11] SNIPS_SLOT_VALUE_TYPE_MUSICARTIST
  | [@as 12] SNIPS_SLOT_VALUE_TYPE_MUSICTRACK
  | [@as 13] SNIPS_SLOT_VALUE_TYPE_CITY
  | [@as 14] SNIPS_SLOT_VALUE_TYPE_COUNTRY
  | [@as 15] SNIPS_SLOT_VALUE_TYPE_REGION;