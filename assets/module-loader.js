var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var Module = {
    noInitialRun: true,
    preRun: [ function() {
	// add libvlc options:
	var flag = 0

	args = document.getElementById("libvlc_opts").value.split(" ")
	for (index in args) {
	    if ( args[index].includes("emscriptenfs") === false) {
		document.getElementById("fileManager").style.display = "none";
		document.getElementById("nativeFsFileManager").style.display = "none";
	    }
	    Module["arguments"].push(args[index]);
	}
	document.getElementById("libvlc_opts").addEventListener('change', function(event) {
	    args = document.getElementById("libvlc_opts").value.split(" ")
	    for (index in args) {
		if ( args[index].includes("emscriptenfs") === false) {
		    document.getElementById("fileManager").style.display = "none";
		    document.getElementById("nativeFsFileManager").style.display = "none";
		}
		Module["arguments"].push(args[index]);
	    }
	    alert("libvlc set with options: " + Module["arguments"] + "please reload the page if you want to change them again.")
	    // noInitialRun is set, we call the main here: 
	    if (flag == 0) {
		callMain(Module["arguments"])
		flag = 1;
	    }
	});
	document.getElementById("libvlc_init").addEventListener('click', function(event) {
	    // make sure main is not called again
	    if (flag == 0) {
		callMain(Module["arguments"])
		flag = 1;
	    }
	});
	if (window.chooseFileSystemEntries === undefined) {
	    document.getElementById("nativeFsFileManager").style.display = "none";
	}
	else {
	    document.getElementById("fileManager").style.display = "none";
	}
	document.getElementById("fileManager").addEventListener('change', function(event) {
	    Module["fileHandle"] = event.target.files[0];
	    console.log("file api : sent file handle all pthread workers");
	}, false);
	document.getElementById("nativeFsFileManager").addEventListener('click', async (e) => {
	    Module["fileHandle"] = await window.chooseFileSystemEntries();
	    console.log("native fs: sent file handle all pthread workers");
	});	
    }],
    postRun: [ function() {
	// this should be set only if offscreen_canvas is activated
/*	var bitmapcontext = document.getElementById("canvas").getContext("bitmaprenderer");
	addEventListener('worker_message', function(msg) {
	    var bitmap = msg.detail;
	    bitmapcontext.transferFromImageBitmap(bitmap);
	});
	// This should run after the wasm module is instantiated
	// before, the Pthread object won't be available
	PThread.receiveObjectTransfer = function (data) {
	    let event = new CustomEvent('worker_message', {
		detail: data.msg
	    });
	    window.dispatchEvent(event);
	}
  */  }],
    fileHandle: undefined,
    print: (function() {
    var element = document.getElementById('output');
    if (element) element.value = ''; // clear browser cache
    return function(text) {
      if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
      // These replacements are necessary if you render to raw HTML
      //text = text.replace(/&/g, "&amp;");
      //text = text.replace(/</g, "&lt;");
      //text = text.replace(/>/g, "&gt;");
      //text = text.replace('\n', '<br>', 'g');
      console.log(text);
      if (element) {
        element.value += text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
      }
    };
  })(),
  printErr: function(text) {
    if (arguments.length > 1) text = Array.prototype.slice.call(arguments).join(' ');
    console.error(text);
  },
  canvas: (function() {
    var canvas = document.getElementById('canvas')
    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false)
    return canvas;
  })(),
  setStatus: function(text) {
    if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
    if (text === Module.setStatus.last.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - Module.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    Module.setStatus.last.time = now;
    Module.setStatus.last.text = text;
    if (m) {
      text = m[1];
      progressElement.value = parseInt(m[2])*100;
      progressElement.max = parseInt(m[4])*100;
      progressElement.hidden = false;
      spinnerElement.hidden = false;
    } else {
      progressElement.value = null;
      progressElement.max = null;
      progressElement.hidden = true;
      if (!text) spinnerElement.style.display = 'none';
    }
    statusElement.innerHTML = text;
  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};
Module.setStatus('Downloading...');
window.onerror = function(event) {
  // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
  Module.setStatus('Exception thrown, see JavaScript console');
  spinnerElement.style.display = 'none';
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};
