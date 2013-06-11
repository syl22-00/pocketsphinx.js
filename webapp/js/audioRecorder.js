(function(window){
    var AUDIO_RECORDER_WORKER = 'js/audioRecorderWorker.js';
    var AudioRecorder = function(source, destination, cfg) {
	recognizer = destination;
	var config = cfg || {};
	var bufferLen = config.bufferLen || 4096;
	var outputBufferLength = config.outputBufferLength || 16000;
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
	var recognizer;
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

	this.start = function(){
	    recognizer.start();
	    recording = true;
	};
	
	this.stop = function(){
	    recording = false;
	    recognizer.stop();
	    worker.postMessage({ command: 'clear' });
	};

	this.cancel = function(){
	    recording = false;
	    recognizer.stop();
	    worker.postMessage({ command: 'clear' });
	};

	worker.onmessage = function(e){
	    if ((e.data.command == 'newBuffer') && recording) {
		recognizer.process(e.data.data);
		__log("New hyp: " + recognizer.getHyp());
	    }
	};
	source.connect(this.node);
	this.node.connect(this.context.destination);
    };
    window.AudioRecorder = AudioRecorder;

})(window);
