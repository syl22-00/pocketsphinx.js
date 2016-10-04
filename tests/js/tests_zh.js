QUnit.module("Data structures");
QUnit.test( "ReturnType type", function(assert) {
    var x = Module.ReturnType.SUCCESS;
    assert.ok(x != undefined, "valid ReturnType should not be undefined" );
    assert.equal(x, Module.ReturnType.SUCCESS, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.BAD_STATE;
    assert.ok(x != undefined, "valid ReturnType should not be undefined" );
    assert.equal(x, Module.ReturnType.BAD_STATE, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.BAD_ARGUMENT;
    assert.ok(x != undefined, "valid ReturnType should not be undefined" );
    assert.equal(x, Module.ReturnType.BAD_ARGUMENT, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.RUNTIME_ERROR;
    assert.ok(x != undefined, "valid ReturnType should not be undefined" );
    assert.equal(x, Module.ReturnType.RUNTIME_ERROR, "valid ReturnType should be equal to itself");
    assert.ok(x != Module.ReturnType.BAD_ARGUMENT, "Inequality between different returntype values");
    assert.ok( Module.ReturnType.BAD_ARGUMENT != Module.ReturnType.BAD_STATE, "Inequality between different returntype values");
    assert.ok( Module.ReturnType.BAD_ARGUMENT != Module.ReturnType.SUCCESS, "Inequality between different returntype values");
    assert.ok( Module.ReturnType.RUNTIME_ERROR != Module.ReturnType.SUCCESS, "Inequality between different returntype values");
    assert.ok( Module.ReturnType.RUNTIME_ERRO == undefined, "Invalid ReturnType should be undefined");
    assert.ok( Module.ReturnType.RUNTIME == undefined, "Invalid ReturnType should be undefined");
    assert.ok( Module.ReturnType.HELLO == undefined, "Invalid ReturnType should be undefined");
});

QUnit.test( "VectorWords type", function(assert) {
    var x = new Module.VectorWords();
    assert.ok(x != undefined, "vectorwords should not be undefined");
    assert.equal(x.size(), 0, "vectorwords should be initialized empty");
    x.push_back(["",""]);
    assert.equal(x.size(), 1, "vectorwords size should grow");
    assert.equal(x.get(0)[0], "", "vectorwords element should have the correct value");
    assert.equal(x.get(0)[1], "", "vectorwords element should have the correct value");
    x.set(0,["1","2"]);
    assert.equal(x.get(0)[0], "1", "vectorwords element should have the correct value");
    assert.equal(x.get(0)[1], "2", "vectorwords element should have the correct value");
    x.set(1,["3","4"]);
    assert.equal(x.get(1), undefined, "vectorwords elements should be undefined");
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.test( "Config type", function(assert) {
    var x = new Module.Config();
    assert.ok(x != undefined, "config should not be undefined");
    assert.equal(x.size(), 0, "config should be initialized empty");
    x.push_back(["",""]);
    assert.equal(x.size(), 1, "config size should grow");
    assert.equal(x.get(0)[0], "", "config element should have the correct value");
    assert.equal(x.get(0)[1], "", "config element should have the correct value");
    x.set(0,["1","2"]);
    assert.equal(x.get(0)[0], "1", "config element should have the correct value");
    assert.equal(x.get(0)[1], "2", "config element should have the correct value");
    x.set(1,["3","4"]);
    assert.equal(x.get(1), undefined, "config elements should be undefined");
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.test( "Transitions types", function(assert) {
    var x = new Module.VectorTransitions();
    assert.ok(x != undefined, "vector transitions should not be undefined");
    assert.equal(x.size(), 0, "vector transitions should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({from: "", to: "", logp: "", hello: ""});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(["", "", "", ""]);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(undefined);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");


    error = undefined;
    try {x.push_back({from: "", to: 2, logp: 0, word: ""});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({from: 0, to: 1, logp: 0, word: 2});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindingError exception");

    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    assert.equal(x.size(), 1, "config size should grow");
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    assert.equal(x.size(), 11, "config size should grow");

    x.push_back({from: 12, to: 13, logp: 0, word: "hello, world!", hello: 0});
    assert.equal(x.size(), 12, "Extra fields in transition should not matter");
    assert.equal(x.get(11).from, 12, "stored transitions should have the correct value");
    assert.equal(x.get(11).to, 13, "stored transitions should have the correct value");
    assert.equal(x.get(11).word, "hello, world!", "stored transitions should have the correct value");
    assert.equal(x.get(11).hello, undefined, "stored transitions should have the correct value");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.test( "AudioBuffer type", function(assert) {
    var x = new Module.AudioBuffer();
    assert.ok(x != undefined, "audio buffer should not be undefined");
    assert.equal(x.size(), 0, "audio buffer should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    var error = undefined;
    try {x.push_back(-33000);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(33000);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back([0]);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back("0");}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    for (var i=0;i<300;i++) {
	x.push_back(i*100);
	x.push_back(-i*100);
    }
    assert.equal(x.size(), 600, "config size should grow");

    x.push_back(12345);
    assert.equal(x.size(), 601, "size of vector should grow accordingly");
    assert.equal(x.get(600), 12345, "stored values should have the correct value");
    assert.equal(x.get(601), undefined, "non-existent values should be undefined");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.test( "Integers type", function(assert) {
    var x = new Module.Integers();
    assert.ok(x != undefined, "Integers should not be undefined");
    assert.equal(x.size(), 0, "Integers should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    var error = undefined;
    try {x.push_back(1e10);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(-1e10);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back([0]);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back("0");}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    assert.equal(error.name, "TypeError", "Should be a TypeError exception");

    for (var i=0;i<300;i++) {
	x.push_back(i*10000);
	x.push_back(-i*10000);
    }
    assert.equal(x.size(), 600, "config size should grow");

    x.push_back(12345678);
    assert.equal(x.size(), 601, "size of vector should grow accordingly");
    assert.equal(x.get(600), 12345678, "stored values should have the correct value");
    assert.equal(x.get(601), undefined, "non-existent values should be undefined");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted vector should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.module("Recognizer type");
QUnit.test( "Recognizer instantiation", function(assert) {
    var x = new Module.Recognizer();
    assert.ok(x != undefined, "New recognizer should not be undefined");
    assert.equal(x.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted recognizer should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

QUnit.test( "Recognizer and configs", function(assert) {
    var y = new Module.Config();
    assert.ok(y != undefined, "New config should not be undefined");
    y.push_back(["",""]);
    var x = new Module.Recognizer(y);
    var words = new Module.VectorWords();
    words.push_back(["ni", "n i"]);
    assert.equal(x.addWords(words), Module.ReturnType.BAD_STATE, "Recognizer should not be valid");
    y.set(0, ["-fwdflat", "no"]);
    assert.equal(x.reInit(y), Module.ReturnType.SUCCESS, "Re-init with valid config should work");
    assert.equal(x.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    assert.ok(error != undefined, "Using a deleted recognizer should raise an exception");
    assert.equal(error.name, "BindingError", "Should be a BindError exception");
});

var recognizer;
var buffer;
var words;
var transitions;
var ids;

QUnit.module( "With living recognizer", {
    beforeEach: function(assert) {
	recognizer = new Module.Recognizer();
	buffer = new Module.AudioBuffer();
	words = new Module.VectorWords();
	transitions = new Module.VectorTransitions();
	ids = new Module.Integers();
	assert.ok( recognizer != undefined, "Recognizer instantiated successfully" );
	assert.ok( true, "one extra assert per test" );
    }, afterEach: function() {
	recognizer.delete();
	buffer.delete();
	words.delete();
	transitions.delete();
	ids.delete();
    }
});
QUnit.test( "Recognizing silence", function(assert) {
    var num_samples = 1024;
    for (var i = 0 ; i < num_samples ; i++) buffer.push_back(0);
    words.push_back(["ni", "n i"]);
    recognizer.addWords(words);
    transitions.push_back({from: 0, to: 0, logp: 0, word: "ni"});
    recognizer.addGrammar(ids, {numStates: 1, start: 0, end: 0, transitions: transitions});
    assert.equal(recognizer.start(), Module.ReturnType.SUCCESS, "Recognizer should start successfully");
    var num_buffers = 64;
    for (var j = 0 ; j < num_buffers ; j++) {
	assert.equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
	assert.equal(recognizer.getHyp(), "", "Recognizer should recognize nothing with silence");
    }
    assert.equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
});
/*
// TODO: make such test in Chinese
QUnit.test( "Recognizing audio", function(assert) {

    for (var i = 0; i < wordList.length; i++) {
	words.push_back(wordList[i]);
    }

    recognizer.addWords(words);
    for (var i = 0; i < grammarOses.transitions.length; i++) {
	transitions.push_back(grammarOses.transitions[i]);
    }
    recognizer.addGrammar(ids, {numStates: grammarOses.numStates, 
				start: grammarOses.start, end: grammarOses.end,
				transitions: transitions});
    for (var i = 0 ; i < audio.length ; i++) buffer.push_back(audio[i]);

    recognizer.start();
    assert.equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    assert.equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    assert.equal(recognizer.getHyp(), "WINDOWS SUCKS AND LINUX IS GREAT", "Recognizer should recognize the correct utterance");
});
*/

QUnit.test("Dictionary words", function(assert) {
    words.push_back(["ni", "n i"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["B", "!"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["C", ""]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["D", "Q"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["E", "n"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["E", "i"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["E(2)", "i"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["F(2)", "n"]);
    assert.equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
});

