test("Recognizer initialization", function() {
    var m = wrapModule();
    equal(m.psGetState(), 0, "Recognizer should first be at uninitalized state");
    var key_ptr = Module.allocate(intArrayFromString("-fwdflat"), 'i8', ALLOC_STACK);
    var value_ptr = Module.allocate(intArrayFromString("no"), 'i8', ALLOC_STACK);
    equal(m.psSetParam(key_ptr, value_ptr), 0, "We should be able to set recognizer parameters before initialization");
    equal(m.psInitialize(), 0, "Recognizer initialization should return 0");
    equal(m.psGetState(), 1, "Recognizer should be at initalized state after initialization");
});
