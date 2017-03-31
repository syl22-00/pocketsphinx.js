(function(window) {
  
  function LocalConsumer() {
  };
  
  LocalConsumer.prototype.postMessage = function(cmd) {
    this[cmd.command](cmd.data);
  };
  
  LocalConsumer.prototype.start = function() { };
  LocalConsumer.prototype.stop = function() { };
  LocalConsumer.prototype.process = function() { };
  
  function VisualizerConsumer(canvas) {
    this.canvas = canvas;
  }
  VisualizerConsumer.prototype = new LocalConsumer();
  VisualizerConsumer.prototype.constructor = VisualizerConsumer;
  
  VisualizerConsumer.prototype.process = function(buffer) {
    
    var ctx = this.canvas.getContext('2d');
    
    var w = this.canvas.width;
    var h = this.canvas.height;
    
    ctx.fillStyle = 'rgb(200, 200, 200)';
    ctx.fillRect(0, 0, w, h);
    
    ctx.lineWidth = 2;
    ctx.strokeStyle = 'rgb(0, 0, 0)';

    ctx.beginPath();
    
    var sliceWidth = w * 1.0 / buffer.length;
    var x = 0;
    
    for(var i = 0; i < buffer.length; i++) {
    
      var v = buffer[i] / 128.0;
      var y = v * h/2;

      if(i === 0) {
        ctx.moveTo(x, y);
      } else {
        ctx.lineTo(x, y);
      }
      
      x += sliceWidth;
    }
    
    ctx.lineTo(w, h/2);
    ctx.stroke();
    
  };
  
  function WavExportConsumer(config) {
    this.sampleRate = config.sampleRate;
    this.maxLength = (config.durationSeconds || 5) * config.sampleRate;
    this.len = 0;
    this.buffers = [];
    this.onFinish = config.onFinish || function(){};
    this.isRecording = true; //auto-start
    this.loop = config.loop || false;
  }
  
  WavExportConsumer.prototype = new LocalConsumer();
  WavExportConsumer.prototype.constructor = WavExportConsumer;
  
  WavExportConsumer.prototype.start = function() {
    this.isRecording = true;
  };
  
  WavExportConsumer.prototype.stop = function() {
    this.done(false);
  };
  
  WavExportConsumer.prototype.done = function(loop) {
    var wav = this.encodeWAV(this.buffers);
    this.onFinish(wav);
    this.buffers = [];
    this.len = 0;
    this.isRecording = loop;
  };
  
  WavExportConsumer.prototype.process = function(buffer) {
    
    if(this.isRecording) {
      this.len += buffer.length;
      //avoid overhead of one giant array, no reason to reallocate what's already there
      this.buffers.push(buffer);
      
      if(this.len >= this.maxLength) {
        this.done(this.loop);
      }
    }
  };
  
  //The following portions for WavExportConsumer are from
  //https://github.com/mattdiamond/Recorderjs
  //Copyright © 2013 Matt Diamond License MIT
  WavExportConsumer.prototype.writeString = function(view, offset, string) {
      for (var i = 0; i < string.length; i++) {
          view.setUint8(offset + i, string.charCodeAt(i));
      }
  };
  
  WavExportConsumer.prototype.encodeWAV = function(samples) {
      var len = Math.min(this.len, this.maxLength);
      var buffer = new ArrayBuffer(44 + len * 2);
      var view = new DataView(buffer);
      
      var numChannels = 1;
      /* RIFF identifier */
      this.writeString(view, 0, 'RIFF');
      /* RIFF chunk length */
      view.setUint32(4, 36 + len * 2, true);
      /* RIFF type */
      this.writeString(view, 8, 'WAVE');
      /* format chunk identifier */
      this.writeString(view, 12, 'fmt ');
      /* format chunk length */
      view.setUint32(16, 16, true);
      /* sample format (raw) */
      view.setUint16(20, 1, true);
      /* channel count */
      view.setUint16(22, numChannels, true);
      /* sample rate */
      view.setUint32(24, this.sampleRate, true);
      /* byte rate (sample rate * block align) */
      view.setUint32(28, this.sampleRate * 4, true);
      /* block align (channel count * bytes per sample) */
      view.setUint16(32, numChannels * 2, true);
      /* bits per sample */
      view.setUint16(34, 16, true);
      /* data chunk identifier */
      this.writeString(view, 36, 'data');
      /* data chunk length */
      view.setUint32(40, len * 2, true);
      
      this.buffersToView(view, 44, samples);
      
      return view;
  };
  
  WavExportConsumer.prototype.buffersToView = function(output, offset, input) {
    var i = 0;
    var cap = Math.min(this.len, this.maxLength);
    for(var bufIdx=0; bufIdx < this.buffers.length && i < cap; bufIdx++) {
      var buffer = input[bufIdx];
      for (var j = 0; j < buffer.length && i < cap; j++, i++, offset += 2) {
          output.setInt16(offset, buffer[j], true);
      }
    }
  };
  
	window.VisualizerConsumer = VisualizerConsumer;
	window.WavExportConsumer = WavExportConsumer;
})(window);
