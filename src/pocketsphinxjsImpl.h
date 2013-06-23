#include <stdint.h>

int psGetStateImpl();
const char* psGetHypImpl();
int psInitializeImpl();
int psStartGrammarImpl(int numStates);
int psEndGrammarImpl(int32_t *id);
int psSwitchGrammarImpl(int id);
int psAddWordImpl(char *word, char *pronunciation);
int psAddTransitionImpl(int fromState, int toState, char *word);
int psStartImpl();
int psStopImpl();
int psProcessImpl(void* data, int length);
