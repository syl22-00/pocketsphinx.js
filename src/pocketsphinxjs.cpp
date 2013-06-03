/*****************************************
 *
 * This is the publci interface of PocketSphinx.js
 * The following functions can be accessed from 
 * JavaScript.
 *
 * It is a C wrapper to avoid name-mangling
 *
 *****************************************/

#include "pocketsphinxjsImpl.h"

extern "C" {
  int psGetState() {
    return psGetStateImpl();
  }
  int psInitialize() {
    return psInitializeImpl();
  }
  int psStartGrammar(int firstState, int lastState) {
    return psStartGrammarImpl(firstState, lastState);
  }
  int psEndGrammar() {
    return psEndGrammarImpl();
  }
  int psAddWord(char *word, char *pronunciation) {
    return psAddWordImpl(word, pronunciation);
  }
  int psAddTransition(int fromState, int toState) {
    return psAddTransitionImpl(fromState, toState);
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
