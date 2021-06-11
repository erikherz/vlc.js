var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var VlcModuleExt = {
  preRun: [],
  postRun: [ function() {
    // This should run after the wasm module is instantiated
    // before, the Pthread object won't be available
    VlcModuleExt.PThread.receiveObjectTransfer = function (data) {
      // Transfer messages from worker threads to the main window.
      let event = new CustomEvent('worker_message', {
        detail: data.msg
      });
      window.dispatchEvent(event);
    };
  }],

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
    var overlay = document.getElementById('overlay')
    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

    overlay.addEventListener('click', (event) => {
      window.on_overlay_click(overlay, event);
    });

    // Create a global window.display_overlay variable to track whether
    // the UI should be visible - see update_overlay function
    window.display_overlay = true,
    overlay.addEventListener('mouseenter', e => {
      window.display_overlay = true;
    });
    overlay.addEventListener('mouseleave', e => {
      window.display_overlay = false;
    });

    return canvas;
  })(),
  setStatus: function(text) {
    if (!VlcModuleExt.setStatus.last) VlcModuleExt.setStatus.last = { time: Date.now(), text: '' };
    if (text === VlcModuleExt.setStatus.last.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - VlcModuleExt.setStatus.last.time < 30) return; // if this is a progress update, skip it if too soon
    VlcModuleExt.setStatus.last.time = now;
    VlcModuleExt.setStatus.last.text = text;
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
    VlcModuleExt.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};
