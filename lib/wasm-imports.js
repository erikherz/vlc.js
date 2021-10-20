// Functions injected into the wasm binary, can be
// called from C/C++ code.
// The top level of this JS file is executed at compile time.
// Functions bodies are copy-pasted into 'experimental.js'
// (unless they're culled from dead code elimination)

mergeInto(LibraryManager.library, {
  // Apply underlying changes (eg video is paused) to the displayed UI
  update_overlay: function() {
    const overlay = document.getElementById("overlay");
    update_overlay(overlay);
  },
});
