test( "Module public functions", function() {
    ok(Module.cwrap('psGetState'), "psGetState should be in the module");
    ok(Module.cwrap('psSetParam', 'number', ['number','number']), "psSetParam should be in the module");
    ok(Module.cwrap('psResetParams', 'number'), "psResetParams should be in the module");
    ok(Module.cwrap('psGetHyp', 'string'), "psGetHyp should be in the module");
    ok(Module.cwrap('psInitialize'), "psInitialize should be in the module");
    ok(Module.cwrap('psStartGrammar', 'number', ['number']), "psStartGrammar should be in the module");
    ok(Module.cwrap('psEndGrammar', 'number', ['number']), "psEndGrammar should be in the module");
    ok(Module.cwrap('psSwitchGrammar', 'number', ['number']), "psSwitchGrammar should be in the module");
    ok(Module.cwrap('psAddWord', 'number', ['number','number']), "psAddWord should be in the module");
    ok(Module.cwrap('psAddTransition', 'number', ['number','number','number']), "psAddTransition should be in the module");
    ok(Module.cwrap('psStart'), "psStart should be in the module");
    ok(Module.cwrap('psStop'), "psStop should be in the module");
    ok(Module.cwrap('psProcess', 'number', ['number','number']), "psProcess should be in the module");
    ok(Module.cwrap('malloc', 'number', ['number']), "malloc should be in the module");
    ok(Module.cwrap('free', 'number', ['number']), "free should be in the module");
});
test( "Module non-existent functions", function() {
    // Not sure about the best way to test these, but this seems to be working
    var hasThrown = false;
    try {Module.cwrap('psGet');}
    catch(e) {hasThrown = true;}
    ok(hasThrown, "psGet should not be in the module");
    
    hasThrown = false;
    try {Module.cwrap('', 'number', ['number','number','number']);}
    catch(e) {hasThrown = true;}
    ok(hasThrown, "empty function name should not be in module");
});
test("Valid calls before initialization", function() {
    var m = wrapModule();
    equal(m.psGetState(), 0, "Recognizer should be uninitalized at first");
    equal(m.psGetHyp(), "", "Hypothesis should be empty at first");
    var key_ptr = Module.allocate(intArrayFromString("my_key"), 'i8', ALLOC_STACK);
    var value_ptr = Module.allocate(intArrayFromString("my_value"), 'i8', ALLOC_STACK);
    equal(m.psSetParam(key_ptr, value_ptr), 0, "We should be able to set recognizer parameters before initialization");
    equal(m.psResetParams(), 0, "We should be able to reset recognizer parameters before initialization");
});
test("Invalid calls before initialization", function() {
    var m = wrapModule();
    equal(m.psStartGrammar(0), 1, "Recognizer should be initalized to start grammar");
    equal(m.psStartGrammar(1), 1, "Recognizer should be initalized to start grammar");
    equal(m.psStartGrammar(100), 1, "Recognizer should be initalized to start grammar");
    equal(m.psEndGrammar(m.c_malloc(4)), 1, "Recognizer should be initalized to end grammar");
    equal(m.psSwitchGrammar(0), 1, "Recognizer should be initalized to switch grammar");
    equal(m.psAddWord(0, 0), 1, "Recognizer should be initalized to add word");
    equal(m.psAddTransition(0, 0, 0), 1, "Recognizer should be initalized to add transitions");
    equal(m.psStart(), 1, "Recognizer should be initalized to start recognition");
    equal(m.psStop(), 1, "Recognizer should be initalized to stop recognition");
    equal(m.psProcess(0, 0), 1, "Recognizer should be initalized to process audio data");
});
