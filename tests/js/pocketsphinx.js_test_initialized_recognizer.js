var m;
module("Tests about recognizer initialization", {
    setup: function() {
	m = wrapModule();
	ok((m.psGetState() == 0) || (m.psGetState() == 1), "we should start with uninitialized or initialized recogniser");
	if (m.psGetState() == 0)  equal(m.psInitialize(), 0, "Recognizer initialization should return 0");
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
    equal(m.psGetState(), 1, "Recognizer should still be initialized");
    equal(m.psGetHyp(), "", "Hypothesis should still be empty")
});
