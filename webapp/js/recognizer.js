(function(window){

    var RecognizerLoader = function() {
	this.load = function(clb, opt) {
	    var psScript = document.createElement('script');
	    psScript.src = "js/pocketsphinx.js";
	    var callback = clb;
	    var output = opt;
	    psScript.onload = psScript.onreadystatechange = function() {
		var rs = this.readyState;
		if (rs) if (rs != 'complete') if (rs != 'loaded') {
			    if(output) output("ERROR");
			    return;
			}
		try {
		    if(callback) callback();
		} catch(e) {
		    if(output) output("ERROR-CALLBACK");
		    return;
		}
		if(output) output("DONE");
	    };
	    var scr = document.getElementsByTagName('script')[0], par = scr.parentNode; par.insertBefore(psScript, scr);
	};
    };

    window.RecognizerLoader = RecognizerLoader;

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
    }

    window.Recognizer = Recognizer;
})(window);
