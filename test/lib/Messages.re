open HermesReason;
open Structs;

let sessionStarted =
  CSessionStartedMessage.{
    reactivated_from_session_id: None,
    site_id: "site id",
    custom_data: None,
    session_id: "session id",
  };

let sessionQueued =
  CSessionQueuedMessage.{
    session_id: "Session id",
    custom_data: Some("Custom data"),
    site_id: "Site id",
  };

let sessionEnded =
  CSessionEndedMessage.{
    session_id: "677a2717-7ac8-44f8-9013-db2222f7923d",
    custom_data: None,
    termination: {
      termination_type: SNIPS_SESSION_TERMINATION_TYPE_ERROR,
      data: Some("Error message"),
      component: SNIPS_HERMES_COMPONENT_NONE,
    },
    site_id: "default",
  };

let intentNotRecognized =
  CIntentNotRecognizedMessage.{
    session_id: "677a2717-7ac8-44f8-9013-db2222f7923d",
    custom_data: Some("data"),
    site_id: "site id",
    input: Some("Hello world"),
    confidence_score: 0.5,
    alternatives: Some([]),
  };

let intentMessage =
  CIntentMessage.{
    session_id: "677a2717-7ac8-44f8-9013-db2222f7923d",
    custom_data: Some("customThing"),
    site_id: "default",
    input: "moi du vert",
    intent: {
      intent_name: "jelb:lightsColor",
      confidence_score: 0.5,
    },
    asr_confidence: 0.5,
    asr_tokens:
      Some([
        [
          {
            value: "moi",
            confidence: 0.5,
            range_start: Int32.of_int @@ 0,
            range_end: Int32.of_int @@ 3,
            time: {
              start: 0.5,
              end_: 1.,
            },
          },
          {
            value: "du",
            confidence: 0.5,
            range_start: Int32.of_int @@ 4,
            range_end: Int32.of_int @@ 6,
            time: {
              start: 1.,
              end_: 1.5,
            },
          },
          {
            value: "vert",
            confidence: 0.5,
            range_start: Int32.of_int @@ 7,
            range_end: Int32.of_int @@ 11,
            time: {
              start: 1.5,
              end_: 2.5,
            },
          },
        ],
      ]),
    slots:
      Some([
        {
          confidence_score: 0.5,
          raw_value: "vert",
          value: {
            value_type: SNIPS_SLOT_VALUE_TYPE_CUSTOM,
            value: String("vert"),
          },
          alternatives: [
            {
              value_type: SNIPS_SLOT_VALUE_TYPE_CUSTOM,
              value: String("blue"),
            },
          ],
          range_start: Int32.of_int @@ 7,
          range_end: Int32.of_int @@ 11,
          entity: "Color",
          slot_name: "Color",
        },
      ]),
    alternatives:
      Some([
        {
          intent_name: Some("alternativeIntent"),
          confidence_score: 0.5,
          slots:
            Some([
              {
                confidence_score: 0.5,
                raw_value: "blue",
                value: {
                  value_type: SNIPS_SLOT_VALUE_TYPE_CUSTOM,
                  value: String("blue"),
                },
                alternatives: [],
                range_start: Int32.of_int @@ 7,
                range_end: Int32.of_int @@ 11,
                entity: "Color",
                slot_name: "Color",
              },
            ]),
        },
      ]),
  };

let injectionStatus =
  CInjectionStatusMessage.{
    last_injection_date: "2018-12-10T11:14:08.468+00:00",
  };

let injectionComplete = CInjectionCompleteMessage.{request_id: "id"};

let injectionResetComplete =
  CInjectionResetCompleteMessage.{request_id: "id"};

let siteMessage =
  CSiteMessage.{site_id: "default", session_id: Some("session id")};