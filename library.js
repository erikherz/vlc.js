mergeInto(LibraryManager.library, {
  update_overlay: function() {
    update_overlay();
  },

  // Worker functions
  on_position_changed: function() {
    postMessage({ cmd: "objectTransfer", msg: "on_position_changed" });
  }
});
