module("Keyword spotting")

test( "Key phrase and dictionary", function() {
    var x = new Module.Recognizer();
    var words = new Module.VectorWords();
    words.push_back(["ONE", "AH"]);
    words.push_back(["TWO", "AH"]);
    words.push_back(["THREE", "AH"]);
    equal(x.addWords(words), Module.ReturnType.SUCCESS, "Recognizer should be valid");
    equal(x.getHyp(), "", "Initial hyp should be empty");
    var ids = new Module.Integers();
    equal(x.addKeyword(ids, "ONE"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    equal(x.addKeyword(ids, "ONE TWO"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    equal(x.addKeyword(ids, "ONE TWO THREE"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    equal(x.addKeyword(ids, "FOUR"), Module.ReturnType.RUNTIME_ERROR, "Invalid key phrase should be added successfully");
    x.delete();
    ids.delete();
    words.delete();
});

var recognizer;
var buffer;
var words;
var ids;

module( "With living recognizer", {
    setup: function() {
	recognizer = new Module.Recognizer();
	buffer = new Module.AudioBuffer();
	words = new Module.VectorWords();
	ids = new Module.Integers();
	ok(recognizer != undefined, "Recognizer instantiated successfully");
    }, 
    teardown: function() {
	recognizer.delete();
	buffer.delete();
	words.delete();
	ids.delete();
    }
});

test( "Spotting silence", function() {
    var num_samples = 512;
    for (var i = 0 ; i < num_samples ; i++) buffer.push_back(0);
    words.push_back(["AH", "AH"]);
    recognizer.addWords(words);
    equal(Module.ReturnType.SUCCESS, recognizer.addKeyword(ids, "AH"), "Key phrase should be added successfully");
    equal(recognizer.start(), Module.ReturnType.SUCCESS, "Recognizer should start successfully");
    var num_buffers = 32;
    for (var j = 0 ; j < num_buffers ; j++) {
	equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
	equal(recognizer.getHyp().length, 0, "Recognizer should spot nothing with silence");
    }
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
});

test( "Spotting audio", function() {

    words.push_back( ["AH", "AH"]);
    recognizer.addWords(words);
    recognizer.addKeyword(ids, "AH");
    for (var i = 0 ; i < audio.length ; i++) buffer.push_back(audio[i]);
    recognizer.start();
    equal(recognizer.getHyp().length, 0, "Recognizer should initially have spotted nothing");
    equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    ok((recognizer.getHyp().length > 0), "Recognizer should have spotted word");
    equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    ok((recognizer.getHyp().length > 0), "Recognizer should have spotted word");
});
