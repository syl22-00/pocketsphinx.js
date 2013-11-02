(function(window){
    var AUDIO_RECORDER_WORKER = 'js/audioRecorderWorker.js';
    var AudioRecorder = function(source, cfg) {
	this.recognizer = null;
	var config = cfg || {};
	var errorCallback = config.errorCallback || function() {};
	var bufferLen = config.bufferLen || 4096;
	var outputBufferLength = config.outputBufferLength || 4000;
	this.context = source.context;
	this.node = this.context.createScriptProcessor(bufferLen);
	var worker = new Worker(config.worker || AUDIO_RECORDER_WORKER);
	worker.postMessage({
	    command: 'init',
	    config: {
		sampleRate: this.context.sampleRate,
		outputBufferLength: outputBufferLength
	    }
	});
	var recording = false;
	this.node.onaudioprocess = function(e) {
	    if (!recording) return;
	    worker.postMessage({
		command: 'record',
		buffer: [
		    e.inputBuffer.getChannelData(0),
		    e.inputBuffer.getChannelData(1)
		]
	    });
	};
	this.start = function(data) {
	    if (this.recognizer) {
                recognizer.postMessage({ command: 'start', data: data });
		recording = true;
		return true;
	    }
	    return false;
	};
	this.stop = function() {
	    if (recording && this.recognizer) {
                recognizer.postMessage({ command: 'stop' });
		recording = false;
	    }
	    worker.postMessage({ command: 'clear' });
	};
	this.cancel = function() {
	    this.stop();
	};
	myClosure = this;
	worker.onmessage = function(e) {
	    if (e.data.error && (e.data.error == "silent")) errorCallback("silent");
	    if ((e.data.command == 'newBuffer') && recording) {
                myClosure.recognizer.postMessage({ command: 'process', data: e.data.data });
	    }
	};
	source.connect(this.node);
	this.node.connect(this.context.destination);
    };
    window.AudioRecorder = AudioRecorder;
})(window);
