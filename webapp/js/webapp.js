(function(window){
    var PS = function() {
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
	    var buffer = Module.allocate(array.length,
					 'i16', ALLOC_NORMAL);
	    for (var i = 0 ; i < array.length ; i++)
		setValue(buffer + i*2, array[i], 'i16');
	    return psProcess(buffer, array.length);
	}
    }

    window.PS = PS;
})(window);
