function startup(onMessage) {
    self.onmessage = function(event) {
	self.onmessage = onMessage;
	self.postMessage();
    }
};

startup(function(event) {
    switch(event.data.command){
    case 'initialize':
	initialize(event.data.callbackId);
	break;
    case 'addWords':
	addWords(event.data.data, event.data.callbackId);
	break;
    case 'addGrammar':
	addGrammar(event.data.data, event.data.callbackId);
	break;
    case 'start':
	start();
	break;
    case 'stop':
	stop();
	break;
    case 'process':
	process(event.data.data);
	break;
    }
});

importScripts('pocketsphinx.js');

var recognizer;
var mySelf = this;

var post = function(message) {
    mySelf.postMessage(message);
};


var Recognizer = function() {
    psGetState = Module.cwrap('psGetState');
    psGetHyp = Module.cwrap('psGetHyp', 'string');
    psInitialize = Module.cwrap('psInitialize');
    psStartGrammar = Module.cwrap('psStartGrammar', 'number', ['number']);
    psEndGrammar = Module.cwrap('psEndGrammar');
    psAddWord = Module.cwrap('psAddWord', 'number', ['number','number']);
    psAddTransition = Module.cwrap('psAddTransition', 'number', ['number','number','number']);
    psStart = Module.cwrap('psStart');
    psStop = Module.cwrap('psStop');
    psProcess = Module.cwrap('psProcess', 'number', ['number','number']);
    c_malloc = Module.cwrap('malloc', 'number', ['number']);
    c_free = Module.cwrap('free', 'number', ['number']);
    
    this.getState = function() {
	return psGetState();
    }
    this.getHyp = function() {
	return psGetHyp();
    }
    this.initialize = function() {
	return psInitialize();
    }
    this.startGrammar = function(numStates) {
	return psStartGrammar(numStates);
    }
    this.endGrammar = function() {
	return psEndGrammar();
    }
    this.addWord = function(word, pronunciation) {
	var word_ptr = Module.allocate(intArrayFromString(word),
				       'i8', ALLOC_STACK);
	var pron_ptr = Module.allocate(intArrayFromString(pronunciation),
				       'i8', ALLOC_STACK);
	return psAddWord(word_ptr, pron_ptr);
    }
    this.addTransition = function(from, to, word) {
	var word_ptr = Module.allocate(intArrayFromString(word),
				       'i8', ALLOC_STACK);
	return psAddTransition(from, to, word_ptr);
    }
    this.start = function() {
	return psStart();
    }
    this.stop = function() {
	return psStop();
    }
    this.process = function(array) {
	var buffer = c_malloc(2 * array.length);
	for (var i = 0 ; i < array.length ; i++)
	    setValue(buffer + i*2, array[i], 'i16');
	var out = psProcess(buffer, array.length);
	c_free(buffer);
	return out;
    }
};

function initialize(clbId) {
    if (recognizer == null)
        recognizer = new Recognizer();
    var initStatus = recognizer.initialize();
    if (initStatus != 0) {
	post({status: "error", command: "initialize", code: initStatus});
    } else {
	post({status: "done", command: "initialize", id: clbId});
    }
};

function addWords(data, clbId) {
    if (recognizer) {
	while (data.length > 0) {
	    var w = data.pop();
	    if (w.length == 2) {
		var output = recognizer.addWord(w[0], w[1]);
		if (output != 0)
		    post({status: "error", command: "addWords", code: output});
	    } else { 
		post({status: "error", command: "addWords", code: "js-data"});
	    }
	}
        post({id: clbId});
    } else {
	post({status: "error", command: "addWords", code: "js-no-recognizer"});
    }
};    

function addGrammar(data, clbId) {
    var output;
    if (recognizer) {
	if (data.hasOwnProperty('numStates') && data.numStates > 0) {
	    output = recognizer.startGrammar(data.numStates);
	    if (output != 0) {
		post({status: "error", command: "addGrammar", code: output});
		return;
	    }
	    if (data.hasOwnProperty('transitions') && (data.transitions.length > 0)) {
		while (data.transitions.length > 0) {
		    var t = data.transitions.pop();
		    if (t.hasOwnProperty('from') && t.hasOwnProperty('to') && t.hasOwnProperty('word')) {
			output = recognizer.addTransition(t.from, t.to, t.word);
			if (output != 0) {
			    post({status: "error", command: "addGrammar", code: output});
			    return;
			}
		    } else {
			post({status: "error", command: "addGrammar", code: "js-data"});
			return;
		    }
		}
	    } else {
		post({status: "error", command: "addGrammar", code: "js-data"});
		return;

	    }
	    recognizer.endGrammar();
	} else {
	    post({status: "error", command: "addGrammar", code: "js-data"});
	    return;
	}
	post({id: clbId, status: "done", command: "addGrammar"});
    } else {
	post({status: "error", command: "addGrammar", code: "js-no-recognizer"});
    }
};

function start() {
    if (recognizer) {
	var output = recognizer.start();
	if (output != 0)
	    post({status: "error", command: "start", code: output});
    } else {
	post({status: "error", command: "start", code: "js-no-recognizer"});
    }
};

function stop() {
    if (recognizer) {
	var output = recognizer.stop();
	if (output != 0)
	    post({status: "error", command: "stop", code: output});
	else 
	    post({hyp: recognizer.getHyp(), final: true});
    } else {
	post({status: "error", command: "stop", code: "js-no-recognizer"});
    }
};

function process(array) {
    if (recognizer) {
	var output = recognizer.process(array);
	if (output != 0)
	    postMessage({status: "error", command: "process", code: output});
	else 
	    post({hyp: recognizer.getHyp()}); 
    } else {
	post({status: "error", command: "process", code: "js-no-recognizer"});
    }
};
