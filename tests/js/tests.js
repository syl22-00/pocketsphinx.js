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
    try {x.push_back({from: "", to: "", hello: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(["", "", ""]);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back(undefined);}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");


    error = undefined;
    try {x.push_back({from: "", to: 2, word: ""});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "TypeError", "Should be a TypeError exception");

    error = undefined;
    try {x.push_back({from: 0, to: 1, word: 2});}
    catch (e) {error = e;}
    ok(error != undefined, "Adding an invalid value to vector should raise an exception");
    equal(error.name, "BindingError", "Should be a BindingError exception");

    x.push_back({from: 0, to: 0, word: ""});
    equal(x.size(), 1, "config size should grow");
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    x.push_back({from: 0, to: 0, word: ""});
    equal(x.size(), 11, "config size should grow");

    x.push_back({from: 12, to: 13, word: "hello, world!", hello: 0});
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




var recognizer;

module( "With living recognizer", {
    setup: function() {
	recognizer = new Module.Recognizer();
	ok( recognizer != undefined, "Recognizer instantiated successfully" );
	ok( true, "one extra assert per test" );
    }, teardown: function() {
	recognizer.delete();
    }
});
test( "test with setup and teardown", function() {
    expect( 2 );
});
