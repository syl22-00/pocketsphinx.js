#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "pocketsphinx.h"

#define MAX_HYP_LENGTH = 1024;

enum PsReturnType {SUCCESS = 0,
		   BAD_STATE = 1,
		   BAD_ARGUMENT = 2,
		   RUNTIME_ERROR = 3};

enum PsState {UNINITIALIZED = 0,
	      INITIALIZED = 1,
	      INITIALIZING = 2,
	      ENTERING_GRAMMAR = 3,
	      RECORDING = 4};

static PsState psState = UNINITIALIZED;

static ps_decoder_t * recognizer = NULL;
static fsg_set_t * grammar_set = NULL;
static logmath_t * logmath = NULL;
static int grammar_index = 0;
static fsg_model_t * current_grammar = NULL;
static std::vector<std::string> grammar_names;
static std::string current_hyp;
static int32 score;
static char const * sentence_id;

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
  logmath = logmath_init(1.0001, 0, 0);
  if (logmath == NULL) {
    std::cout << "logmath is NULL\n";
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  grammar_set = ps_update_fsgset(recognizer);
  //grammar_set = ps_get_fsgset(recognizer);
  if (grammar_set == NULL) {
    std::cout << "grammar_set is NULL\n";
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  // If we're there, it means we're successful
  std::cout << "Done\n";
  psState = INITIALIZED;
  current_hyp = "";
  return SUCCESS;
}

int psGetStateImpl() {return psState;}

const char* psGetHypImpl() {
  return current_hyp.c_str();
}

int psStartGrammarImpl(int numStates) {
  if (psState != INITIALIZED)
    return BAD_STATE;
  std::ostringstream grammar_name;
  grammar_name << "grammar-" << grammar_index;
  grammar_names.push_back(grammar_name.str());
  current_grammar = fsg_model_init(grammar_names.back().c_str(), logmath, 1.0, numStates);
  if (current_grammar == NULL)
    return RUNTIME_ERROR;
  current_grammar->start_state = 0;
  current_grammar->final_state = numStates - 1;
  psState = ENTERING_GRAMMAR;
  return SUCCESS;
}

int psEndGrammarImpl() {
  if (psState != ENTERING_GRAMMAR)
    return BAD_STATE;
  fsg_model_add_silence(current_grammar, "<sil>", -1, 1.0);
  if (current_grammar != fsg_set_add(grammar_set, grammar_names.back().c_str(), current_grammar)) {
    return RUNTIME_ERROR;
  }
  grammar_index++;
  ps_update_fsgset(recognizer);
  fsg_model_t * fsg = fsg_set_select(grammar_set, grammar_names.back().c_str());
  if (fsg == NULL)
    return RUNTIME_ERROR;
  if (ps_update_fsgset(recognizer) == NULL)
    return RUNTIME_ERROR;
  psState = INITIALIZED;
  return SUCCESS;
}

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

int psAddTransitionImpl(int fromState, int toState, char* word) {
  if (psState != ENTERING_GRAMMAR)
    return BAD_STATE;
  if ((fromState > current_grammar->final_state) || (toState > current_grammar->final_state))
    return BAD_ARGUMENT;
  fsg_model_trans_add(current_grammar, fromState, toState, 0, fsg_model_word_add(current_grammar, word));
  return SUCCESS;
}

int psStartImpl() {
  if (psState != INITIALIZED)
    return BAD_STATE;
  if (ps_start_utt(recognizer, NULL) < 0) {
    return RUNTIME_ERROR;
  }
  current_hyp = "";
  psState = RECORDING;
  return SUCCESS;
}

int psStopImpl() {
  if (psState != RECORDING)
    return BAD_STATE;
  if (ps_end_utt(recognizer) < 0) {
    return RUNTIME_ERROR;
  }
  const char* h = ps_get_hyp(recognizer, &score, &sentence_id);
  current_hyp = (h == NULL) ? "" : h;
  psState = INITIALIZED;
  return SUCCESS;
}

int psProcessImpl(void* data, int length) {
  if (psState != RECORDING)
    return BAD_STATE;
  if ((data == NULL) || (length == 0))
    return RUNTIME_ERROR;
  ps_process_raw(recognizer, (short int *) data, length, 0, 0);
  /*
  short int * dataShort = (short int *) data;
  for (int i = 0; i< length ; ++i)
    std::cout << dataShort[i] << "\n";
  */
  const char* h = ps_get_hyp(recognizer, &score, &sentence_id);
  current_hyp = (h == NULL) ? "" : h;
  return SUCCESS;
}
