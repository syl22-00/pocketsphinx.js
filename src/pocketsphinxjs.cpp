/*****************************************
 *
 * This is the public interface of Pocketsphinx.js
 * The following functions can be accessed from 
 * JavaScript.
 *
 * It is a C wrapper around our C++ implementation 
 * to avoid name-mangling
 *
 *****************************************/

#include "pocketsphinxjsImpl.h"

extern "C" {
  int psGetState() {
    return psGetStateImpl();
  }
  const char* psGetHyp() {
    return psGetHypImpl();
  }
  int psInitialize() {
    return psInitializeImpl();
  }
  int psStartGrammar(int numStates) {
    return psStartGrammarImpl(numStates);
  }
  int psEndGrammar(int32_t *id) {
    return psEndGrammarImpl(id);
  }
  int psSwitchGrammar(int id) {
    return psSwitchGrammarImpl(id);
  }
  int psAddWord(char *word, char *pronunciation) {
    return psAddWordImpl(word, pronunciation);
  }
  int psAddTransition(int fromState, int toState, char *word) {
    return psAddTransitionImpl(fromState, toState, word);
  }
  int psStart() {
    return psStartImpl();
  }
  int psStop() {
    return psStopImpl();
  }
  int psProcess(void* data, int length) {
    return psProcessImpl(data, length);
  }
}
