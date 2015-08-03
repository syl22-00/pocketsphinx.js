module("Data structures");

test( "ReturnType type", function() {
    var x = Module.ReturnType.SUCCESS;
    ok (x != undefined, "valid ReturnType should not be undefined" );
    equal(x, Module.ReturnType.SUCCESS, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.BAD_STATE;
    ok (x != undefined, "valid ReturnType should not be undefined" );
    equal(x, Module.ReturnType.BAD_STATE, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.BAD_ARGUMENT;
    ok (x != undefined, "valid ReturnType should not be undefined" );
    equal(x, Module.ReturnType.BAD_ARGUMENT, "valid ReturnType should be equal to itself");
    x = Module.ReturnType.RUNTIME_ERROR;
    ok (x != undefined, "valid ReturnType should not be undefined" );
    equal(x, Module.ReturnType.RUNTIME_ERROR, "valid ReturnType should be equal to itself");
    ok (x != Module.ReturnType.BAD_ARGUMENT, "Inequality between different returntype values");
    ok ( Module.ReturnType.BAD_ARGUMENT != Module.ReturnType.BAD_STATE, "Inequality between different returntype values");
    ok ( Module.ReturnType.BAD_ARGUMENT != Module.ReturnType.SUCCESS, "Inequality between different returntype values");
    ok ( Module.ReturnType.RUNTIME_ERROR != Module.ReturnType.SUCCESS, "Inequality between different returntype values");
    ok ( Module.ReturnType.RUNTIME_ERRO == undefined, "Invalid ReturnType should be undefined");
    ok ( Module.ReturnType.RUNTIME == undefined, "Invalid ReturnType should be undefined");
    ok ( Module.ReturnType.HELLO == undefined, "Invalid ReturnType should be undefined");
});

test( "VectorWords type", function() {
    var x = new Module.VectorWords();
    ok (x != undefined, "vectorwords should not be undefined");
    equal(x.size(), 0, "vectorwords should be initialized empty");
    x.push_back(["",""]);
    equal(x.size(), 1, "vectorwords size should grow");
    equal(x.get(0)[0], "", "vectorwords element should have the correct value");
    equal(x.get(0)[1], "", "vectorwords element should have the correct value");
    x.set(0,["1","2"]);
    equal(x.get(0)[0], "1", "vectorwords element should have the correct value");
    equal(x.get(0)[1], "2", "vectorwords element should have the correct value");
    x.set(1,["3","4"]);
    equal(x.get(1), undefined, "vectorwords elements should be undefined");
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

test( "Config type", function() {
    var x = new Module.Config();
    ok (x != undefined, "config should not be undefined");
    equal(x.size(), 0, "config should be initialized empty");
    x.push_back(["",""]);
    equal(x.size(), 1, "config size should grow");
    equal(x.get(0)[0], "", "config element should have the correct value");
    equal(x.get(0)[1], "", "config element should have the correct value");
    x.set(0,["1","2"]);
    equal(x.get(0)[0], "1", "config element should have the correct value");
    equal(x.get(0)[1], "2", "config element should have the correct value");
    x.set(1,["3","4"]);
    equal(x.get(1), undefined, "config elements should be undefined");
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

test( "Segmentation type", function() {
    var x = new Module.Segmentation();
    ok (x != undefined, "Segmentation should not be undefined");
    equal(x.size(), 0, "Segmentation should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({word: "", start: "", end: 1, hello: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(["", "", ""]);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(undefined);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({start: "", end: 2, word: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({start: 0, end: 1, word: 2});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to Segmentation should raise an exception");
    equal(error.name, "BindingError", "Should be a BindingError exception");

    x.push_back({start: 0, end: 0, word: ""});
    equal(x.size(), 1, "config size should grow");
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    x.push_back({start: 0, end: 0, word: ""});
    equal(x.size(), 11, "Segmentation size should grow");

    x.push_back({start: 12, end: 13, word: "hello, world!", hello: 0});
    equal(x.size(), 12, "Extra fields in Segmentation item should not matter");
    equal(x.get(11).start, 12, "stored segmentation item should have the correct value");
    equal(x.get(11).end, 13, "stored segmentation item should have the correct value");
    equal(x.get(11).word, "hello, world!", "stored segmentation item should have the correct value");
    equal(x.get(11).hello, undefined, "stored segmentation item should have the correct value");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted Segmentation should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");

});

test( "Transitions types", function() {
    var x = new Module.VectorTransitions();
    ok (x != undefined, "vector transitions should not be undefined");
    equal(x.size(), 0, "vector transitions should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({from: "", to: "", logp: "", hello: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(["", "", "", ""]);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(undefined);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");


    error = undefined;
    try {x.push_back({from: "", to: 2, logp: 0, word: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({from: 0, to: 1, logp: 0, word: 2});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindingError exception");

    x.push_back({from: 0, to: 0, logp: 0, word: ""});
    equal(x.size(), 1, "config size should grow");
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
    equal(x.size(), 11, "config size should grow");

    x.push_back({from: 12, to: 13, logp: 0, word: "hello, world!", hello: 0});
    equal(x.size(), 12, "Extra fields in transition should not matter");
    equal(x.get(11).from, 12, "stored transitions should have the correct value");
    equal(x.get(11).to, 13, "stored transitions should have the correct value");
    equal(x.get(11).word, "hello, world!", "stored transitions should have the correct value");
    equal(x.get(11).hello, undefined, "stored transitions should have the correct value");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

test( "AudioBuffer type", function() {
    var x = new Module.AudioBuffer();
    ok (x != undefined, "audio buffer should not be undefined");
    equal(x.size(), 0, "audio buffer should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    var error = undefined;
    try {x.push_back(-33000);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(33000);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back([0]);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back("0");}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    for (var i=0;i<300;i++) {
	x.push_back(i*100);
	x.push_back(-i*100);
    }
    equal(x.size(), 600, "config size should grow");

    x.push_back(12345);
    equal(x.size(), 601, "size of vector should grow accordingly");
    equal(x.get(600), 12345, "stored values should have the correct value");
    equal(x.get(601), undefined, "non-existent values should be undefined");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

test( "Integers type", function() {
    var x = new Module.Integers();
    ok (x != undefined, "Integers should not be undefined");
    equal(x.size(), 0, "Integers should be initialized empty");
    var error = undefined;
    try {x.push_back({});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    var error = undefined;
    try {x.push_back(1e10);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(-1e10);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back([0]);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back("0");}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    for (var i=0;i<300;i++) {
	x.push_back(i*10000);
	x.push_back(-i*10000);
    }
    equal(x.size(), 600, "config size should grow");

    x.push_back(12345678);
    equal(x.size(), 601, "size of vector should grow accordingly");
    equal(x.get(600), 12345678, "stored values should have the correct value");
    equal(x.get(601), undefined, "non-existent values should be undefined");
    
    x.delete();
    var error = undefined;
    try {x.get(0);}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

module("Recognizer type");
test( "Recognizer instantiation", function() {
    var x = new Module.Recognizer();
    ok (x != undefined, "New recognizer should not be undefined");
    equal(x.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

test( "Instantiation of several recognizers", function() {
    var x = new Module.Recognizer();
    var y = new Module.Recognizer();
    var z = new Module.Recognizer();
    ok (x != undefined, "New recognizer should not be undefined");
    ok (y != undefined, "New recognizer should not be undefined");
    ok (z != undefined, "New recognizer should not be undefined");
    equal(x.getHyp(), "", "Initial hyp should be empty");
    equal(y.getHyp(), "", "Initial hyp should be empty");
    equal(z.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
    equal(y.getHyp(), "", "Initial hyp should still be empty");
    equal(z.getHyp(), "", "Initial hyp should still be empty");
    y.delete();
    error = undefined;
    try {y.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    z.delete();
    error = undefined;
    try {z.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
});

/*
*
* Removing that test
* It seems like every other try, or if the test is run by itself, it passes
* oterwise it is aborted because of the recognizer being initialized with an
* invalid config
*/
/*
test( "Recognizer and configs", function() {
    var y = new Module.Config();
    ok(y != undefined, "New config should not be undefined");
    y.push_back(["",""]);
    var x = new Module.Recognizer(y);
    var words = new Module.VectorWords();
    words.push_back(["AH", "AH"]);
    equal(x.addWords(words), Module.ReturnType.BAD_STATE, "Recognizer should not be valid");
    y.set(0, ["-fwdflat", "no"]);
    equal(x.reInit(y), Module.ReturnType.SUCCESS, "Re-init with valid config should work");
    equal(x.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});
*/

test( "Recognizer and configs", function() {
    var y = new Module.Config();
    ok(y != undefined, "New config should not be undefined");
    var x = new Module.Recognizer(y);
    var words = new Module.VectorWords();
    words.push_back(["AH", "AH"]);
    equal(x.addWords(words), Module.ReturnType.SUCCESS, "Recognizer should be valid");
    y.push_back(["-fwdflat", "no"]);
    equal(x.reInit(y), Module.ReturnType.SUCCESS, "Re-init with valid config should work");
    equal(x.getHyp(), "", "Initial hyp should be empty");
    x.delete();
    y.delete();
    words.delete();
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

module("Recognizer memory");
test( "Multiple recognizer instantiation and deletion", function() {
    var x;
    // The goal of this test was to see if deleting the recognizer
    // was releasing memory. It does not seem to be done synchronously
    // so if that number of recognizers becomes too high, it wont pass 
    for (var i = 0;i<10;i++) {
	x = new Module.Recognizer();
	x.delete();
    }
    var error = undefined;
    try {x.getHyp();}
    catch (e) {error = e;}
    ok(error != undefined, "Using a deleted recognizer should raise an exception");
    equal(error.name, "BindingError", "Should be a BindError exception");
});

var recognizer;
var buffer;
var words;
var transitions;
var ids;
var segmentation;

module( "With living recognizer", {
    setup: function() {
	recognizer = new Module.Recognizer();
	buffer = new Module.AudioBuffer();
	words = new Module.VectorWords();
	transitions = new Module.VectorTransitions();
	ids = new Module.Integers();
	segmentation = new Module.Segmentation();
	ok(recognizer != undefined, "Recognizer instantiated successfully");
    }, 
    teardown: function() {
	recognizer.delete();
	buffer.delete();
	words.delete();
	transitions.delete();
	ids.delete();
	segmentation.delete();
    }
});


test("Dictionary words", function() {
    words.push_back(["A", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["B", "!"]);
    equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["C", ""]);
    equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["D", "Q"]);
    equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["E", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["E", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
    words.set(0,["E(2)", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    words.set(0,["F(2)", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.RUNTIME_ERROR, "Invalid words should not be added successfully");
});

test("Looking up words", function() {
    words.push_back(["A", "AH"]);
    equal(recognizer.addWords(words), Module.ReturnType.SUCCESS, "Valid words should be added successfully");
    equal(recognizer.lookupWord("A"), "AH", "Words in the dictionary should be looked up correctly");
    equal(recognizer.lookupWord("B"), "", "Words not in the dictionary should return empty strings");
})

test("Grammars", function() {
    words.push_back(["A", "AH"]);
    recognizer.addWords(words);
    transitions.push_back({from: 0, to: 0, logp: 0, word: "A"});
    equal(Module.ReturnType.SUCCESS, recognizer.addGrammar(ids, {numStates: 1, start: 0, end: 0, transitions: transitions}), "Grammar should be added successfully");
});

test( "Recognizing silence", function() {
    var num_samples = 1024;
    for (var i = 0 ; i < num_samples ; i++) buffer.push_back(0);
    words.push_back(["AH", "AH"]);
    recognizer.addWords(words);
    transitions.push_back({from: 0, to: 0, logp: 0, word: "AH"});
    equal(Module.ReturnType.SUCCESS, recognizer.addGrammar(ids, {numStates: 1, start: 0, end: 0, transitions: transitions}), "Grammar should be added successfully");
    equal(recognizer.start(), Module.ReturnType.SUCCESS, "Recognizer should start successfully");
    var num_buffers = 64;
    for (var j = 0 ; j < num_buffers ; j++) {
	equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
	equal(recognizer.getHyp(), "", "Recognizer should recognize nothing with silence");
    }
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
});


test( "Switching grammars", function() {

    words.push_back(["A", "AH"]);
    words.push_back(["B", "AH"]);
    words.push_back(["C", "AH"]);
    words.push_back(["D", "AH"]);
    recognizer.addWords(words);
    var transitionA = new Module.VectorTransitions();
    var transitionB = new Module.VectorTransitions();
    var transitionC = new Module.VectorTransitions();
    var transitionD = new Module.VectorTransitions();
    transitionA.push_back({from: 0, to: 0, word: "A", logp: 0});
    transitionB.push_back({from: 0, to: 0, word: "B", logp: 0});
    transitionC.push_back({from: 0, to: 0, word: "C", logp: 0});
    transitionD.push_back({from: 0, to: 0, word: "D", logp: 0});
    equal(recognizer.addGrammar(ids,
				{numStates: 1, start: 0, end: 0,
				 transitions: transitionA}),
	  Module.ReturnType.SUCCESS,
	  "Grammar should be added successfully");
    var idA = ids.get(0);
    
    equal(recognizer.addGrammar(ids,
				{numStates: 1, start: 0, end: 0,
				 transitions: transitionB}),
	  Module.ReturnType.SUCCESS,
	  "Grammar should be added successfully");
    var idB = ids.get(0);
    
    equal(recognizer.addGrammar(ids,
				{numStates: 1, start: 0, end: 0,
				 transitions: transitionC}),
	  Module.ReturnType.SUCCESS,
	  "Grammar should be added successfully");
    var idC = ids.get(0);
    
    equal(recognizer.addGrammar(ids,
				{numStates: 1, start: 0, end: 0,
				 transitions: transitionD}),
	  Module.ReturnType.SUCCESS,
	  "Grammar should be added successfully");
    var idD = ids.get(0);
    
    for (var i = 0 ; i < audio.length ; i++) buffer.push_back(audio[i]);

    recognizer.start();
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok(recognizer.getHyp().indexOf("A") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("B") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("C") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("D") >= 0, "We should get an hyp that matches the grammar");
    equal(recognizer.switchGrammar(idA), Module.ReturnType.SUCCESS, "Recognizer should switch grammar successfully");
    recognizer.start();
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok(recognizer.getHyp().indexOf("A") >= 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("B") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("C") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("D") < 0, "We should get an hyp that matches the grammar");

    equal(recognizer.switchGrammar(idB), Module.ReturnType.SUCCESS, "Recognizer should switch grammar successfully");
    recognizer.start();
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok(recognizer.getHyp().indexOf("A") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("B") >= 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("C") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("D") < 0, "We should get an hyp that matches the grammar");

    equal(recognizer.switchGrammar(idC), Module.ReturnType.SUCCESS, "Recognizer should switch grammar successfully");
    recognizer.start();
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok(recognizer.getHyp().indexOf("A") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("B") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("C") >= 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("D") < 0, "We should get an hyp that matches the grammar");

    equal(recognizer.switchGrammar(idD), Module.ReturnType.SUCCESS, "Recognizer should switch grammar successfully");
    recognizer.start();
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok(recognizer.getHyp().indexOf("A") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("B") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("C") < 0, "We should get an hyp that matches the grammar");
    ok(recognizer.getHyp().indexOf("D") >= 0, "We should get an hyp that matches the grammar");
    transitionA.delete();
    transitionB.delete();
    transitionC.delete();
    transitionD.delete();
});

test( "Recognizing audio", function() {

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
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    equal(recognizer.getHyp(), "WINDOWS SUCKS AND LINUX IS GREAT", "Recognizer should recognize the correct utterance");
    equal(segmentation.size(), 0, "Segmentation should be initialized empty");
    equal(recognizer.getHypseg(segmentation), Module.ReturnType.SUCCESS);
    equal(segmentation.size(), 9, "Segmentation should not be empty after filled in");
    equal(segmentation.get(0).word, "<sil>", "Value stored in Segmentation should be the correct one");
    equal(segmentation.get(0).start, 0, "Value stored in Segmentation should be the correct one");
    equal(segmentation.get(0).end, 16, "Value stored in Segmentation should be the correct one");
    equal(segmentation.get(1).word, "WINDOWS");
    equal(segmentation.get(1).start, 17);
    equal(segmentation.get(1).end, 62);
    equal(segmentation.get(2).word, "SUCKS");
    equal(segmentation.get(2).start, 63);
    equal(segmentation.get(2).end, 119);
    equal(segmentation.get(3).word, "<sil>");
    equal(segmentation.get(3).start, 120);
    equal(segmentation.get(3).end, 129);
    equal(segmentation.get(4).word, "AND(2)");
    equal(segmentation.get(4).start, 130);
    equal(segmentation.get(4).end, 145);
    equal(segmentation.get(5).word, "LINUX");
    equal(segmentation.get(5).start, 146);
    equal(segmentation.get(5).end, 202);
    equal(segmentation.get(6).word, "IS");
    equal(segmentation.get(6).start, 203);
    equal(segmentation.get(6).end, 215);
    equal(segmentation.get(7).word, "GREAT");
    equal(segmentation.get(7).start, 216);
    equal(segmentation.get(7).end, 274);
    equal(segmentation.get(8).word, "<sil>");
    equal(segmentation.get(8).start, 275);
    equal(segmentation.get(8).end, 301);
});
