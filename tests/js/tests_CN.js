module("Chinese Recognizer");
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

var recognizer;
var buffer;

module( "With living Chinese recognizer", {
    setup: function() {
	var config = new Module.Config();
	ok (config != undefined, "New config should not be undefined");
	config.push_back(["-dict","mandarin_notone.dic"]);
	config.push_back(["-hmm","tdt_sc_8k"]);
	config.push_back(["-lm","gigatdt.5000.DMP"]);
	recognizer = new Module.Recognizer(config);
	config.delete();
	buffer = new Module.AudioBuffer();
	ok( recognizer != undefined, "Recognizer instantiated successfully" );
	ok( true, "one extra assert per test" );
    }, teardown: function() {
	recognizer.delete();
	buffer.delete();
    }
});

test( "Recognizing silence", function() {
    var num_samples = 1024;
    for (var i = 0 ; i < num_samples ; i++) buffer.push_back(0);
    equal(recognizer.start(), Module.ReturnType.SUCCESS, "Recognizer should start successfully");
    var num_buffers = 64;
    for (var j = 0 ; j < num_buffers ; j++) {
	equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
	equal(recognizer.getHyp(), "", "Recognizer should recognize nothing with silence");
    }
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
});
