

int psGetStateImpl();
int psInitializeImpl();
int psStartGrammarImpl(int numStates);
int psEndGrammarImpl();
int psAddWordImpl(char *word, char *pronunciation);
int psAddTransitionImpl(int fromState, int toState, char *word);
int psStartImpl();
int psStopImpl();
int psProcessImpl(void* data, int length);
