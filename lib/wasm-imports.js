mergeInto(LibraryManager.library, {
  update_overlay: function() {
    const overlay = document.getElementById("overlay");
    update_overlay(overlay);
  },

  // Worker functions
  on_position_changed: function() {
    postMessage({ cmd: "objectTransfer", msg: "on_position_changed" });
  }
});
