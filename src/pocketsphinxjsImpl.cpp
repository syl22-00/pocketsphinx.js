#include <iostream>
#include "pocketsphinx.h"

enum PsReturnType {SUCCESS = 0,
		   BAD_STATE = 1,
		   RUNTIME_ERROR = 2};

enum PsState {UNINITIALIZED = 0,
	      INITIALIZED = 1,
	      INITIALIZING = 2,
	      ENTERING_GRAMMAR = 3,
	      RECORDING = 4};
static PsState psState = UNINITIALIZED;

static ps_decoder_t * recognizer = NULL;

int psInitializeImpl() {
  if (psState != UNINITIALIZED)
    return BAD_STATE;
  psState = INITIALIZING;
  const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    { "-argfile",
      ARG_STRING,
      NULL,
      "Argument file giving extra arguments." },
    { "-adcdev",
      ARG_STRING,
      NULL,
      "Name of audio device to use for input." },
    { "-infile",
      ARG_STRING,
      NULL,
      "Audio file to transcribe." },
    { "-time",
      ARG_BOOLEAN,
      "no",
      "Print word times in file transcription." },
    CMDLN_EMPTY_OPTION
  };
  // We will initialize the decoder with the provided acoustic model
  char * argv[] = {(char *)"-hmm", (char *)"am/rm1_200", (char *)"-bestpath", (char *)"no"};
  int argc = 4;
  // First, create a config with the provided arguments
  cmd_ln_t * config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
  if (config == NULL) {
    std::cout << "Config is NULL\n";
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  // Now it is time to initialize the decoder
  recognizer = ps_init(config);
  if (recognizer == NULL) {
    std::cout << "recognizer is NULL\n";
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  // If we're there, it means we're successful
  std::cout << "Done\n";
  psState = INITIALIZED;
  return SUCCESS;
}

int psGetStateImpl() {return psState;}
int psNewGrammarImpl(int firstState, int lastState) {return 0;}
int psStartGrammarImpl() {return 0;}
int psAddWordImpl(char *word, char *pronunciation) {
  if (psState != INITIALIZED)
    return BAD_STATE;
  if (recognizer == NULL) {
    std::cout << "Recognizer is null\n";
    return RUNTIME_ERROR;
  }
  if (ps_add_word(recognizer, word, pronunciation, 1) < 0)
    return RUNTIME_ERROR;
  return SUCCESS;
}
int psAddTransitionImpl(int fromState, int toState) {return 0;}
int psStartImpl() {return 0;}
int psStopImpl() {return 0;}
int psProcessImpl(void* data, int length) {
  short int * dataShort = (short int *) data;
  for (int i = 0; i< length ; ++i)
    std::cout << dataShort[i] << "\n";
  return 0;
}
