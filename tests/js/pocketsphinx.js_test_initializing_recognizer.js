test("Recognizer initialization", function() {
    var m = wrapModule();
    equal(m.psGetState(), 0, "Recognizer should first be at uninitalized state");
    equal(m.psInitialize(), 0, "Recognizer initialization should return 0");
    equal(m.psGetState(), 1, "Recognizer should be at initalized state after initialization");
    equal(m.psInitialize(), 0, "Recognizer should be re-initialized successfully");
    equal(m.psGetState(), 1, "Recognizer should remain at initalized state after attempt of re-initialization");
});
