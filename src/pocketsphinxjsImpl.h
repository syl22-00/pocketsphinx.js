

int psGetStateImpl();
int psInitializeImpl();
int psStartGrammarImpl(int firstState, int lastState);
int psEndGrammarImpl();
int psAddWordImpl(char *word, char *pronunciation);
int psAddTransitionImpl(int fromState, int toState);
int psStartImpl();
int psStopImpl();
int psProcessImpl(void* data, int length);
