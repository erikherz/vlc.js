// Re-exports of functions defined in "include/vlc/libvlc/media/player.h"
// See exports_media_player.c for why this is necessary.

#include <vlc/vlc.h>
#include <vlc_common.h>

#include <emscripten.h>
#include <emscripten/html5.h>

libvlc_media_t* EMSCRIPTEN_KEEPALIVE wasm_media_new_path(const char *path) {
    return libvlc_media_new_path(path);
}

libvlc_media_t* EMSCRIPTEN_KEEPALIVE wasm_media_new_location(const char *path) {
    return libvlc_media_new_location(path);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_retain( libvlc_media_t *media) {
  libvlc_media_retain(media);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_release( libvlc_media_t *media) {
  libvlc_media_release(media);
}
