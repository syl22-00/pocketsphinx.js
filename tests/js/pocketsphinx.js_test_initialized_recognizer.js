var m;
module("Tests about recognizer initialized state", {
    setup: function() {
	m = wrapModule();
	ok((m.psGetState() == 0)||(m.psGetState() == 1), "we should start with uninitialized or initialized recogniser");
	if (m.psGetState() == 0) equal(m.psInitialize(), 0, "Recognizer initialization should return 0");
	equal(m.psGetState(), 1, "we should initialized after setup")
    },
    teardown: function() {m = undefined;}
});

test("Invalid calls in initialized state", function() {
    equal(m.psEndGrammar(m.c_malloc(4)), 1, "Recognizer should be at entering_grammar state when ending entering grammar");
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
    equal(m.psAddTransition(0, 0, 0), 1, "Recognizer should be at entering_grammar state when ending entering grammar");
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
});

test("Valid calls in initialized state that stay in initialized state", function() {
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
    equal(m.psGetState(), 1, "Recognizer should still be initialized after multiple calls to psGetState");
    equal(m.psGetHyp(), "", "Hypothesis should be empty");
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
    equal(m.psGetHyp(), "", "Hypothesis should still be empty");
    var word_ptr = Module.allocate(intArrayFromString("A"),
                                   'i8', ALLOC_STACK);
    var pron_ptr = Module.allocate(intArrayFromString("AH"),
                                   'i8', ALLOC_STACK);
    equal(m.psAddWord(word_ptr, pron_ptr), 0, "Should be able to add words");
    var key_ptr = Module.allocate(intArrayFromString("-fwdflat"), 'i8', ALLOC_STACK);
    var value_ptr = Module.allocate(intArrayFromString("no"), 'i8', ALLOC_STACK);
    equal(m.psSetParam(key_ptr, value_ptr), 0, "We should be able to set recognizer parameters before initialization");
    equal(m.psResetParams(), 0, "We should be able to reset recognizer parameters before initialization");
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
    equal(m.psGetHyp(), "", "Hypothesis should still be empty")
});
