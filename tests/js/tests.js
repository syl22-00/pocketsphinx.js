
test( "Initial test", function() {
    var x = new Module.Recognizer();
    ok(x != undefined, "Recognizer instantiated successfully" );
    x.delete();
});

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
    equal(x.get(1), undefined, "vectorwords elements be undefined");
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
