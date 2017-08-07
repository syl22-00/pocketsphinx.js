QUnit.config.autostart = false;
QUnit.module("Keyword spotting")

QUnit.test( "Key phrase and dictionary", function(assert) {
    var x = new Module.Recognizer();
    var words = new Module.VectorWords();
    words.push_back(["ONE", "AH"]);
    words.push_back(["TWO", "AH"]);
    words.push_back(["THREE", "AH"]);
    assert.equal(x.addWords(words), Module.ReturnType.SUCCESS, "Recognizer should be valid");
    assert.equal(x.getHyp(), "", "Initial hyp should be empty");
    var ids = new Module.Integers();
    assert.equal(x.addKeyword(ids, "ONE"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    assert.equal(x.addKeyword(ids, "ONE TWO"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    assert.equal(x.addKeyword(ids, "ONE TWO THREE"), Module.ReturnType.SUCCESS, "Valid key phrase should be added successfully");
    // TODO: this needs to be fixed:
    // assert.equal(x.addKeyword(ids, "FOUR"), Module.ReturnType.RUNTIME_ERROR, "Invalid key phrase should be added successfully");
    x.delete();
    ids.delete();
    words.delete();
});

var recognizer;
var buffer;
var words;
var ids;

QUnit.module( "With living recognizer", {
    beforeEach: function(assert) {
	recognizer = new Module.Recognizer();
	buffer = new Module.AudioBuffer();
	words = new Module.VectorWords();
	ids = new Module.Integers();
	assert.ok(recognizer != undefined, "Recognizer instantiated successfully");
    }, 
    afterEach: function() {
	recognizer.delete();
	buffer.delete();
	words.delete();
	ids.delete();
    }
});

QUnit.test( "Spotting silence", function(assert) {
    var num_samples = 512;
    for (var i = 0 ; i < num_samples ; i++) buffer.push_back(0);
    words.push_back(["AH", "AH"]);
    recognizer.addWords(words);
    assert.equal(Module.ReturnType.SUCCESS, recognizer.addKeyword(ids, "AH"), "Key phrase should be added successfully");
    assert.equal(recognizer.start(), Module.ReturnType.SUCCESS, "Recognizer should start successfully");
    var num_buffers = 32;
    for (var j = 0 ; j < num_buffers ; j++) {
	assert.equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
	assert.equal(recognizer.getHyp().length, 0, "Recognizer should spot nothing with silence");
    }
    assert.equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
});

QUnit.test( "Spotting audio", function(assert) {

    words.push_back( ["AH", "AH"]);
    recognizer.addWords(words);
    recognizer.addKeyword(ids, "AH");
    for (var i = 0 ; i < audio.length ; i++) buffer.push_back(audio[i]);
    recognizer.start();
    assert.equal(recognizer.getHyp().length, 0, "Recognizer should initially have spotted nothing");
    assert.equal(recognizer.process(buffer), Module.ReturnType.SUCCESS, "Recognizer should process successfully");
    assert.ok((recognizer.getHyp().length > 0), "Recognizer should have spotted word");
    assert.equal(recognizer.stop(), Module.ReturnType.SUCCESS, "Recognizer should stop successfully");
    assert.ok((recognizer.getHyp().length > 0), "Recognizer should have spotted word");
});
