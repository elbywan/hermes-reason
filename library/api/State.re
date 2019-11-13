type t = {
  dialog: DialogState.t,
  injection: InjectionState.t,
  soundFeedback: SoundFeedbackState.t,
  tts: TtsState.t,
};

let make = protocolHandler => {
  dialog: DialogState.make(protocolHandler),
  injection: InjectionState.make(protocolHandler),
  soundFeedback: SoundFeedbackState.make(protocolHandler),
  tts: TtsState.make(protocolHandler),
};

let reduce = (state, action) => {
  dialog: action |> DialogState.reduce(state.dialog),
  injection: action |> InjectionState.reduce(state.injection),
  soundFeedback: state.soundFeedback,
  tts: state.tts,
};