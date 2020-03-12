class Processor extends AudioWorkletProcessor {
    constructor() {
        super();
        this.port.onmessage = e => {
            if (e.data.type === "recv-audio-queue") {
                this.buf = e.data.data;
                this.capacity = e.data.sab_size / 4;
                this.head = new Uint32Array(this.buf, e.data.sab_ptr, 1);
                this.tail = new Uint32Array(this.buf, e.data.sab_ptr + 4, 1);
                this.storage = new Float32Array(this.buf, e.data.sab_ptr + 8, this.capacity);
            } else {
                throw "unexpected.";
            }
        };
    }

    process(inputs, outputs, parameters) {
        const output = outputs[0];
        const nbChannels = output.length;
        const nbSamples = output[0].length;

        // The pointer that is writen by the VLC audio output.
        var head = Atomics.load(this.head, 0) / 4;
        // The pointer that is read here.
        var tail = Atomics.load(this.tail, 0) / 4;

        var i = 0;
        /* We read and increment the tail until we reach the head. */
        while (tail != head && i < nbSamples)
        {
            /* Samples are interleaved in the storage.
               We must separate them by channel. */
            for (let c = 0; c < nbChannels; ++c) {
                output[c][i] = this.storage[tail];
                tail++;
                if (tail == this.capacity) {
                    /* We are at the end of the ring buffer
                       We must return to the start. */
                    tail = 0;
                }
            }
            i++;
        }
        Atomics.store(this.tail, 0, tail * 4);

        return true; // Mark for GC
    }
}

registerProcessor("worklet-processor", Processor);
