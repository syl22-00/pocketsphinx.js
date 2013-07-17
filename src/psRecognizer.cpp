#include <vector>
#include <map>
#include <emscripten/bind.h>

struct Transition {
  int from;
  int to;
  std::string word;
};

struct Grammar {
  int start;
  int end;
  std::vector<Transition> transitions;
};

typedef std::map<std::string, std::string> Dictionary;

struct Word {
  std::string word;
  std::string pronunciation;
};

class PSRecognizer {

public: 
  PSRecognizer(int);
  std::string addTransition(const Transition&);
  int addTransitions(const std::vector<Transition>&);
  int addGrammar(const Grammar&);
  std::string addWord(const Word&);
  int getValue();

private:
  int value;
  Dictionary dictionary;

};

PSRecognizer::PSRecognizer(int x) : value(x) { }

int PSRecognizer::getValue() {
  return value;
}

std::string PSRecognizer::addTransition(const Transition& t) {
  return t.word;
}

int PSRecognizer::addTransitions(const std::vector<Transition>& T) {
  return T.size();
}

int PSRecognizer::addGrammar(const Grammar& g) {
  return g.transitions.size();
}

std::string PSRecognizer::addWord(const Word& w) {
  dictionary[w.word] = w.pronunciation;
  return w.word + " - " + w.pronunciation;
}

/**********************************************
 *
 * Usage:
 *
 * var instance = new Module.PSRecognizer(2);
 * instance.addWord(["HELLO", "AH"]);
 * instance.addTransition({from: 1, to: 2, word: "hello"});
 * var transitions = new Module.VectorTransition();
 * transitions.push_back({from: 1, to: 2, word: "hello"});
 * instance.addGrammar({start: 1, end: 2, transitions: transitions});
 *
 *********************************************/

EMSCRIPTEN_BINDINGS(my_class_example) {
  emscripten::value_tuple<Word>("Word")
    .element(&Word::word)
    .element(&Word::pronunciation);

  emscripten::value_struct<Transition>("Transition")
    .field("from", &Transition::from)
    .field("to", &Transition::to)
    .field("word", &Transition::word);

  emscripten::register_vector<Transition>("VectorTransition");

  emscripten::value_struct<Grammar>("Grammar")
    .field("start", &Grammar::start)
    .field("end", &Grammar::end)
    .field("transitions", &Grammar::transitions);

  emscripten::class_<PSRecognizer>("PSRecognizer")
    .constructor<int>()
    .function("addTransition", &PSRecognizer::addTransition)
    .function("addTransitions", &PSRecognizer::addTransitions)
    .function("addGrammar", &PSRecognizer::addGrammar)
    .function("addWord", &PSRecognizer::addWord)
    .function("getValue", &PSRecognizer::getValue);
}
