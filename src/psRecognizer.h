#ifndef _PSRECOGNIqZER_H_
#define _PSRECOGNIZER_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <emscripten/bind.h>
#include "pocketsphinx.h"

namespace pocketsphinxjs {
  
  enum ReturnType {
    SUCCESS,
    BAD_STATE,
    BAD_ARGUMENT,
    RUNTIME_ERROR
  };

  struct Transition {
    int from;
    int to;
    std::string word;
  };

  struct Grammar {
    int start;
    int end;
    int numStates;
    std::vector<Transition> transitions;
  };

  typedef std::map<std::string, std::string> Dictionary;
  
  struct Word {
    std::string word;
    std::string pronunciation;
  };

  struct ConfigItem {
    std::string key;
    std::string value;
  };

  typedef std::vector<std::string> StringsListType;
  typedef std::set<std::string> StringsSetType;
  typedef std::vector<ConfigItem> Config;

  class Recognizer {

  public:
    Recognizer();
    Recognizer(const Config&);
    int reInit(const Config&);
    int addWords(const std::vector<Word>&);
    int addGrammar(int, const Grammar&);
    int switchGrammar(int);
    std::string getHyp();
    int start();
    int stop();
    int process(const std::vector<int16_t>&);
    ~Recognizer();
    
  private:
    ReturnType init(const Config&);
    bool isValidParameter(const std::string&, const std::string&);
    void cleanup();
    StringsListType grammar_names;
    bool is_fsg;
    bool is_recording;
    std::string current_hyp;
    int32_t grammar_index; 
    int32 score;
    char const * sentence_id;
    fsg_model_t * current_grammar;
    ps_decoder_t * decoder;
    fsg_set_t * grammar_set;
    logmath_t * logmath;
    StringsSetType acoustic_models;
    std::string default_acoustic_model;
    StringsSetType language_models;
    StringsSetType dictionaries;
    std::string default_language_model;
    std::string default_dictionary;
 
  };
  
} // namespace pocketsphinxjs




/**********************************************
 *
 * Usage:
 *
 * var recognizer = new Module.Recognizer();
 * var words = new Module.VectorWords();
 * words.push_back(["HELLO", "HH AH L OW"]);
 * words.push_back(["WORLD", "W ER L D"]);
 * recognizer.addWords(words);
 * var transitions = new Module.VectorTransitions();
 * transitions.push_back({from: 0, to: 1, word: "HELLO"});
 * transitions.push_back({from: 1, to: 2, word: "WORLD"});
 * var c_malloc = Module.cwrap('malloc', 'number', ['number']);
 * var id_ptr = c_malloc(4);
 * recognizer.addGrammar(id_ptr, {start: 1, end: 2, numStates: 3, transitions: transitions});
 * var id = getValue(id_ptr, 'i32');
 * var c_free = Module.cwrap('free', 'number', ['number']);
 * c_free(id_ptr);
 * var length = 100;
 * recognizer.start();
 * var buffer = new Module.AudioBuffer();
 * for (var i = 0 ; i < length ; i++)
 *    buffer.push_back(i*100);
 * recognizer.process(buffer);
 * recognizer.process(buffer);
 * recognizer.process(buffer);
 * c_free(buffer);
 * recognizer.stop();
 *
 *********************************************/

namespace ps = pocketsphinxjs;
EMSCRIPTEN_BINDINGS(recognizer) {

  emscripten::enum_<ps::ReturnType>("ReturnType")
    .value("SUCCESS", ps::SUCCESS)
    .value("BAD_STATE", ps::BAD_STATE)
    .value("BAD_ARGUMENT", ps::BAD_ARGUMENT)
    .value("RUNTIME_ERROR", ps::RUNTIME_ERROR);


  emscripten::value_tuple<ps::Word>("Word")
    .element(&ps::Word::word)
    .element(&ps::Word::pronunciation);

  emscripten::value_struct<ps::Transition>("Transition")
    .field("from", &ps::Transition::from)
    .field("to", &ps::Transition::to)
    .field("word", &ps::Transition::word);


  emscripten::register_vector<int16_t>("AudioBuffer");
  emscripten::register_vector<ps::Transition>("VectorTransitions");
  emscripten::register_vector<ps::Word>("VectorWords");

  emscripten::value_struct<ps::Grammar>("Grammar")
    .field("start", &ps::Grammar::start)
    .field("end", &ps::Grammar::end)
    .field("numStates", &ps::Grammar::numStates)
    .field("transitions", &ps::Grammar::transitions);

  emscripten::class_<ps::Recognizer>("Recognizer")
    .constructor<>()
    .constructor<const ps::Config&>()
    .function("reInit", &ps::Recognizer::reInit)
    .function("addWords", &ps::Recognizer::addWords)
    .function("addGrammar", &ps::Recognizer::addGrammar)
    .function("switchGrammar", &ps::Recognizer::switchGrammar)
    .function("getHyp", &ps::Recognizer::getHyp)
    .function("start", &ps::Recognizer::start)
    .function("stop", &ps::Recognizer::stop)
    .function("process", &ps::Recognizer::process);
}

#endif /* _PSRECOGNIZER_H_ */
