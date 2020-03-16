class Processor extends AudioWorkletProcessor {
    constructor() {
	super();
	// this.interleaved = new Float32Array(128);
	this.port.onmessage = e => {
	    if (e.data.type === "recv-audio-queue") {
		this.buf = e.data.data;
		this.capacity = (e.data.sab_size - 8) / 4;
		this.head = new Uint32Array(this.buf, e.data.sab_ptr, 1);
		this.tail = new Uint32Array(this.buf, e.data.sab_ptr + 4, 1);
		this.storage = new Float32Array(this.buf, e.data.sab_ptr + 8, this.capacity);
		// this._audio_reader = new AudioReader(new RingBuffer(e.data.data, Float32Array, e.data.sab_ptr, e.data.sab_size));
	    } else {
              throw "unexpected.";
	    }
	};
    }
    
    process(inputs, outputs, parameters) {
	var next;
	if(Atomics.xor(this.tail, 0, Atomics.load(this.head, 0)) == 0) {
	    console.log("LOG: cannot read..."); // Empty buffer
	}
	next = Atomics.load(this.tail, 0) + 1;
	if (next >= this.capacity) {
	    next = 0;
	}
	for ( var i = 0; i < 128; i++ ) {
	    else {
		
		outputs[0][0][i] = this.storage[Atomics.load(this.tail, 0)];
		Atomics.store(this.tail, 0, next);
	    }
	}
	return true; // Mark for GC
    }
}

registerProcessor("worklet-processor", Processor);
