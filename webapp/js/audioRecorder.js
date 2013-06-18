(function(window){
    var AUDIO_RECORDER_WORKER = 'js/audioRecorderWorker.js';
    var AudioRecorder = function(source, cfg) {
	this.recognizer = null;
	var config = cfg || {};
	var bufferLen = config.bufferLen || 4096;
	var outputBufferLength = config.outputBufferLength || 4000;
	this.context = source.context;
	// Todo, see if we can change input or output channel numbers
	this.node = this.context.createJavaScriptNode(bufferLen, 2, 2);
	var worker = new Worker(config.worker || AUDIO_RECORDER_WORKER);
	worker.postMessage({
	    command: 'init',
	    config: {
		sampleRate: this.context.sampleRate,
		outputBufferLength: outputBufferLength
	    }
	});

	var recording = false;
	this.node.onaudioprocess = function(e){
	    if (!recording) return;
	    worker.postMessage({
		command: 'record',
		buffer: [
		    e.inputBuffer.getChannelData(0),
		    e.inputBuffer.getChannelData(1)
		]
	    });
	};

	this.start = function() {
	    if (this.recognizer) {
		this.recognizer.start();
		recording = true;
		return true;
	    }
	    return false;
	};
	
	this.stop = function() {
	    if (recording && this.recognizer) {
		this.recognizer.stop();
		recording = false;
	    }
	    worker.postMessage({ command: 'clear' });
	};

	this.cancel = function() {
	    this.stop();
	};
	myClosure = this;
	worker.onmessage = function(e){
	    if ((e.data.command == 'newBuffer') && recording) {
		myClosure.recognizer.process(e.data.data);
		updateHyp(myClosure.recognizer.getHyp());
	    }
	};
	source.connect(this.node);
	this.node.connect(this.context.destination);
    };
    window.AudioRecorder = AudioRecorder;
})(window);
