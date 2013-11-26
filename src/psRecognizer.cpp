#include "psRecognizer.h"
#include "pocketsphinx.h"
#include "pocketsphinxjs-config.h"

#define MAX_NUM_GRAMMARS 1000

namespace pocketsphinxjs {
  typedef std::map<std::string, std::string> StringsMapType;
  typedef std::map<std::string, std::string>::iterator StringsMapIterator;

  // Implemented later in this file
  ReturnType parseStringList(const std::string &, StringsSetType*, std::string*);

  Recognizer::Recognizer(): grammar_names(MAX_NUM_GRAMMARS), is_fsg(true), is_recording(false), current_hyp(""), grammar_index(0) {
    Config c;
    if (init(c) != SUCCESS) cleanup();
  }

  Recognizer::Recognizer(const Config& config) : grammar_names(MAX_NUM_GRAMMARS), is_fsg(true), is_recording(false), current_hyp(""), grammar_index(0) {
    if (init(config) != SUCCESS) cleanup();
  }

  ReturnType Recognizer::reInit(const Config& config) {
    ReturnType r = init(config);
    if (r != SUCCESS) cleanup();
    return r;
  }

  ReturnType Recognizer::addWords(const std::vector<Word>& words) {
    if (decoder == NULL) return BAD_STATE;
    for (int i=0 ; i<words.size() ; ++i) {
      // This case is not properly handeled by ps_add_word, so we treat it separately
      if (words.at(i).pronunciation.size() == 0) return RUNTIME_ERROR;
      if (ps_add_word(decoder, words.at(i).word.c_str(), words.at(i).pronunciation.c_str(), 1) < 0) return RUNTIME_ERROR;
    }
    return SUCCESS;
  }

  ReturnType Recognizer::addGrammar(Integers& id, const Grammar& grammar) {
    if (decoder == NULL) return BAD_STATE;
    std::ostringstream grammar_name;
    grammar_name << grammar_index;
    grammar_names.push_back(grammar_name.str());
    current_grammar = fsg_model_init(grammar_names.back().c_str(), logmath, 1.0, grammar.numStates);
    if (current_grammar == NULL)
      return RUNTIME_ERROR;
    current_grammar->start_state = grammar.start;
    current_grammar->final_state = grammar.end;
    for (int i=0;i<grammar.transitions.size();i++) {
      if (grammar.transitions.at(i).word.size() == 0)
	fsg_model_null_trans_add(current_grammar, grammar.transitions.at(i).from, grammar.transitions.at(i).to, grammar.transitions.at(i).logp);
      else
	fsg_model_trans_add(current_grammar, grammar.transitions.at(i).from, grammar.transitions.at(i).to, grammar.transitions.at(i).logp, fsg_model_word_add(current_grammar, grammar.transitions.at(i).word.c_str()));
    }
    fsg_model_add_silence(current_grammar, "<sil>", -1, 1.0);

    if(ps_set_fsg(decoder, grammar_names.back().c_str(), current_grammar)) {
      return RUNTIME_ERROR;
    }
    if (id.size() == 0) id.push_back(grammar_index);
    else id.at(0) = grammar_index;
    grammar_index++;
    // We switch to the newly added grammar right away
    if (ps_set_search(decoder, grammar_names.back().c_str())) {
      return RUNTIME_ERROR;
    }
    return SUCCESS;
  }

  ReturnType Recognizer::switchGrammar(int id) {
    if (decoder == NULL) return BAD_STATE;
    std::ostringstream grammar_name;
    grammar_name << id;
    if(ps_set_search(decoder, grammar_name.str().c_str())) {
      return RUNTIME_ERROR;
    }
    return SUCCESS;
  }

  ReturnType Recognizer::start() {
    if ((decoder == NULL) || (is_recording)) return BAD_STATE;
    if (ps_start_utt(decoder, NULL) < 0) {
      return RUNTIME_ERROR;
    }
    current_hyp = "";
    is_recording = true;
    return SUCCESS;
  }

  ReturnType Recognizer::stop() {
    if ((decoder == NULL) || (!is_recording)) return BAD_STATE;
    if (ps_end_utt(decoder) < 0) {
      return RUNTIME_ERROR;
    }
    const char* h = ps_get_hyp(decoder, &score, &sentence_id);
    current_hyp = (h == NULL) ? "" : h;
    is_recording = false;
    return SUCCESS;
  }

  ReturnType Recognizer::process(const std::vector<int16_t>& buffer) {
    if ((decoder == NULL) || (!is_recording)) return BAD_STATE;
    if (buffer.size() == 0)
      return RUNTIME_ERROR;
    ps_process_raw(decoder, (short int *) &buffer[0], buffer.size(), 0, 0);
    const char* h = ps_get_hyp(decoder, &score, &sentence_id);
    current_hyp = (h == NULL) ? "" : h;
    return SUCCESS;
  }

  Recognizer::~Recognizer() {
    cleanup();
  }

  std::string Recognizer::getHyp() {
    return current_hyp;
  }

  void Recognizer::cleanup() {
    if (decoder) ps_free(decoder);
    if (logmath) logmath_free(logmath);
    decoder = NULL;
    logmath = NULL;
  }

  ReturnType Recognizer::init(const Config& config) {
    parseStringList(HMM_FOLDERS, &acoustic_models, &default_acoustic_model);
#ifdef LM_FILES
    parseStringList(LM_FILES, &language_models, &default_language_model);
#endif /* LM_FILES */
#ifdef DICT_FILES
    parseStringList(DICT_FILES, &dictionaries, &default_dictionary);
#endif /* DICT_FILES */

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

    std::map<std::string, std::string> parameters;
    for (int i=0 ; i< config.size() ; ++i)
      parameters[config[i].key] = config[i].value;
    
    if ((parameters.find("-hmm") == parameters.end()) || (acoustic_models.find(parameters["-hmm"]) == acoustic_models.end()))
      parameters["-hmm"] = default_acoustic_model;
    if (parameters.find("-bestpath") == parameters.end())
      parameters["-bestpath"] = "no";
    if (parameters.find("-remove_noise") == parameters.end())
      parameters["-remove_noise"] = "no";

    int argc = 2 * parameters.size();
    char ** argv = new char*[argc];
    int index = 0;
    for (StringsMapIterator i = parameters.begin() ; i != parameters.end(); ++i) {
      if (isValidParameter(i->first, i->second)) {
	if (i->first == "-lm") is_fsg = false;
	argv[index++] = (char*) i->first.c_str();
	argv[index++] = (char*) i->second.c_str();
      }
    }

    cmd_ln_t * cmd_line = cmd_ln_parse_r(NULL, cont_args_def, argc, argv, FALSE);
    if (cmd_line == NULL) {
      delete [] argv;
      return RUNTIME_ERROR;
    }
    decoder = ps_init(cmd_line);
    delete [] argv;
    if (decoder == NULL) {
      return RUNTIME_ERROR;
    }
    logmath = logmath_init(1.0001, 0, 0);
    if (logmath == NULL) {
      return RUNTIME_ERROR;
    }
    return SUCCESS;
  }

  bool Recognizer::isValidParameter(const std::string & key, const std::string & value) {
    if ((key == "-dict") && (dictionaries.find(value) == dictionaries.end())) return false;
    if ((key == "-lm") && (language_models.find(value) == language_models.end())) return false;
    return true;
  }

  /*******************************************
   *
   * Parses the string with available models and fills in
   * the default model and the available models
   * @param string to parse, the models are separated with ;
   * @return 0 if successful, alaways successful
   *
   *****************************************/
  ReturnType parseStringList(const std::string & list, StringsSetType* strings_set, std::string* default_string = NULL) {
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
} // namespace pocketsphinxjs
