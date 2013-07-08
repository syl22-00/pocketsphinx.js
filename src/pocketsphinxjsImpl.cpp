/****************************************************
 *
 * This file includes the implementation of the functions
 * made available to JavaScript. They are mainly convenience
 * functions around PocketSphinx API.
 *
 **************************************************/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include "pocketsphinx.h"
#include "pocketsphinxjs-config.h"

typedef std::vector<std::string> StringsListType;
typedef std::set<std::string> StringsSetType;
typedef std::map<std::string, std::string> StringsMapType;
typedef std::map<std::string, std::string>::iterator StringsMapIterator;

/** Possible return values of most calls */
enum PsReturnType {SUCCESS = 0,
		   BAD_STATE = 1,
		   BAD_ARGUMENT = 2,
		   RUNTIME_ERROR = 3};

/** The recognizer uses static variables,
 * we keep track of the state
 */
enum PsState {UNINITIALIZED = 0,
	      INITIALIZED = 1,
	      INITIALIZING = 2,
	      ENTERING_GRAMMAR = 3,
	      RECORDING = 4};

static PsState psState = UNINITIALIZED;

// Implementation is based on static variables,
// there is no thread issue as there is no data
// shared between threads in JavaScript
static ps_decoder_t * recognizer = NULL;
static fsg_set_t * grammar_set = NULL;
static logmath_t * logmath = NULL;
static int32_t grammar_index = 0;
static fsg_model_t * current_grammar = NULL;
static StringsListType grammar_names;
static StringsSetType acoustic_models;
static StringsSetType language_models;
static StringsSetType dictionaries;
static std::string default_acoustic_model;
static std::string default_language_model;
static std::string default_dictionary;
static std::string current_hyp;
static StringsMapType recognizer_parameters; 
static int32 score;
static char const * sentence_id;
static bool is_fsg;
// Implemented later in this file
int parseStringList(const std::string &, StringsSetType*, std::string*);
bool isValidParameter(const std::string &, const std::string &);
/**********************************
 *
 * This initializes a new recognizer
 *
 * @return 0 if successful, error code otherwise
 *
 *********************************/
int psInitializeImpl() {
  if ((psState != UNINITIALIZED) && (psState != INITIALIZED))
    return BAD_STATE;
  psState = INITIALIZING;
  // The acoustic models packaged should be added to our set
  if (default_acoustic_model.size() == 0)
    parseStringList(HMM_FOLDERS, &acoustic_models, &default_acoustic_model);

#ifdef LM_FILES
  if (default_language_model.size() == 0)
    parseStringList(LM_FILES, &language_models, &default_language_model);
#endif /* LM_FILES */

#ifdef DICT_FILES
  if (default_dictionary.size() == 0)
    parseStringList(DICT_FILES, &dictionaries, &default_dictionary);
#endif /* DICT_FILES */

  // TODO: we need to keep grammar names in scope
  // while the recognizer is alive. Storing the strings
  // in a vector is fine until it gets expanded, making new
  // copies of the strings, invalidating the char* pointers
  grammar_names.reserve(1000);
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
  if ((recognizer_parameters.find("-hmm") == recognizer_parameters.end()) || (acoustic_models.find(recognizer_parameters["-hmm"]) == acoustic_models.end()))
    recognizer_parameters["-hmm"] = default_acoustic_model;
  if (recognizer_parameters.find("-bestpath") == recognizer_parameters.end())
    recognizer_parameters["-bestpath"] = "no";
  int argc = 2 * recognizer_parameters.size();
  char ** argv = new char*[argc];
  int index = 0;
  is_fsg = true;
  for (StringsMapIterator i = recognizer_parameters.begin() ; i != recognizer_parameters.end(); ++i) {
    if (isValidParameter(i->first, i->second)) {
      if (i->first == "-lm") is_fsg = false;
      argv[index++] = (char*) i->first.c_str();
      argv[index++] = (char*) i->second.c_str();
    }
  }
  // First, create a config with the provided arguments  
  cmd_ln_t * config = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
  if (config == NULL) {
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  // Now it is time to initialize the decoder
  if (recognizer == NULL)
    recognizer = ps_init(config);
  else
    if (ps_reinit(recognizer, config) != 0) {
      psState = UNINITIALIZED;
      return RUNTIME_ERROR;
    }
  if (recognizer == NULL) {
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  if (is_fsg) {
    grammar_set = ps_update_fsgset(recognizer);
    if (grammar_set == NULL) {
      return RUNTIME_ERROR;
    }
  }
  logmath = logmath_init(1.0001, 0, 0);
  if (logmath == NULL) {
    psState = UNINITIALIZED;
    return RUNTIME_ERROR;
  }
  psState = INITIALIZED;
  current_hyp = "";
  return SUCCESS;
}

/***********************************************
 *
 * Sets a recognizer parameter
 * @param key of the key/value pair to pass
 * @param value of the key/value pair to pass
 * @return 0 if successful, error code otherwise
 *
 ***********************************************/
int psSetParamImpl(char* key, char* value) {
  std::string myKey = key;
  std::string myValue = value;
  if (myKey.size() == 0)
    return BAD_ARGUMENT;
  recognizer_parameters[myKey] = myValue;
  return SUCCESS;
}

/***********************************************
 *
 * Resets (i.e clears) all previously entered
 * recognizer parameter
 * @return 0 if successful, error code otherwise
 *
 ***********************************************/
int psResetParamsImpl() {
  recognizer_parameters.clear();
  return SUCCESS;
}

/**************************************
 *
 * Retrieves the current state
 * @return current state
 *
 *************************************/
int psGetStateImpl() {return psState;}

/**************************************
 *
 * Retrieves the current hypothesis
 * @return current hypothesis
 *
 *************************************/
const char* psGetHypImpl() {
  return current_hyp.c_str();
}

/***********************************
 *
 * Starts inputing a new grammar
 *
 * @param numStates, the number of states of the grammar
 * @return 0 if successful, error code otherwise
 * 
 ***********************************/
int psStartGrammarImpl(int numStates) {
  if (psState != INITIALIZED)
    return BAD_STATE;
  std::ostringstream grammar_name;
  grammar_name << grammar_index;
  grammar_names.push_back(grammar_name.str());
  current_grammar = fsg_model_init(grammar_names.back().c_str(), logmath, 1.0, numStates);
  if (current_grammar == NULL)
    return RUNTIME_ERROR;
  current_grammar->start_state = 0;
  current_grammar->final_state = numStates - 1;
  psState = ENTERING_GRAMMAR;
  return SUCCESS;
}


/*******************************************
 *
 * Finishes inputing a grammar
 * @param pointer to host the value of the id of the grammar
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
int psEndGrammarImpl(int32_t *id) {
  if (psState != ENTERING_GRAMMAR)
    return BAD_STATE;
  fsg_model_add_silence(current_grammar, "<sil>", -1, 1.0);
  if (current_grammar != fsg_set_add(grammar_set, grammar_names.back().c_str(), current_grammar)) {
    return RUNTIME_ERROR;
  }
  *id = grammar_index;
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

/*******************************************
 *
 * Switches the recognizer to the given grammar
 * @param id of the grammar to switch to
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
int psSwitchGrammarImpl(int id) {
  if (psState != INITIALIZED)
    return BAD_STATE;
  std::ostringstream grammar_name;
  grammar_name << id;
  fsg_model_t * fsg = fsg_set_select(grammar_set, grammar_name.str().c_str());
  if (fsg == NULL)
    return RUNTIME_ERROR;
  if (ps_update_fsgset(recognizer) == NULL)
    return RUNTIME_ERROR;
  return SUCCESS;
}

/*******************************************
 *
 * Adds a new word to the dictionary
 * @param word to add
 * @param pronunciation of the word to add
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
int psAddWordImpl(char *word, char *pronunciation) {
  if (psState != INITIALIZED)
    return BAD_STATE;
  if (recognizer == NULL) {
    return RUNTIME_ERROR;
  }
  if (ps_add_word(recognizer, word, pronunciation, 1) < 0)
    return RUNTIME_ERROR;
  return SUCCESS;
}

/*******************************************
 *
 * Adds a transition to the grammar currently being entered
 * @param starting state of the transition
 * @param destination state of the transition
 * @param word associated to the transition
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
int psAddTransitionImpl(int fromState, int toState, char* word) {
  if (psState != ENTERING_GRAMMAR)
    return BAD_STATE;
  if ((fromState > current_grammar->final_state) || (toState > current_grammar->final_state))
    return BAD_ARGUMENT;
  fsg_model_trans_add(current_grammar, fromState, toState, 0, fsg_model_word_add(current_grammar, word));
  return SUCCESS;
}

/*******************************************
 *
 * Starts recognition on the current grammar
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
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

/*******************************************
 *
 * Stops recognition
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
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

/*******************************************
 *
 * Processes the given audio samples
 * @param pointer ot an array of audio samples (short integers)
 * @param size of the array
 * @return 0 if successful, error code otherwise
 *
 *****************************************/
int psProcessImpl(void* data, int length) {
  if (psState != RECORDING)
    return BAD_STATE;
  if ((data == NULL) || (length == 0))
    return RUNTIME_ERROR;
  ps_process_raw(recognizer, (short int *) data, length, 0, 0);
  const char* h = ps_get_hyp(recognizer, &score, &sentence_id);
  current_hyp = (h == NULL) ? "" : h;
  return SUCCESS;
}

/*******************************************
 *
 * Parses the string with available acoustic model and fills in
 * the default model and the available models
 * @param string to parse, the models are separated with ;
 * @return 0 if successful, alaways successful
 *
 *****************************************/
int parseStringList(const std::string & list, StringsSetType* strings_set, std::string* default_string = NULL) {
  if ((strings_set == NULL) || (list.size() == 0))
    return BAD_ARGUMENT;
  std::string separator = ";";
  std::string::size_type index;
  std::string first_string = list.substr(0, index = list.find(separator));
  if (default_string != NULL)
    *default_string = first_string;
  strings_set->insert(first_string);
  while((index != std::string::npos) && (index != list.size() -1))
    strings_set->insert(list.substr(index+1, -1 -index +(index = list.find(separator, index + 1))));
  return SUCCESS;
}

bool isValidParameter(const std::string & key, const std::string & value) {
  if ((key == "-dict") && (dictionaries.find(value) == dictionaries.end())) return false;
  if ((key == "-lm") && (language_models.find(value) == language_models.end())) return false;
  return true;
}
