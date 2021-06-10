#include <vlc/vlc.h>
#include <vlc_common.h>

#include <emscripten.h>
#include <emscripten/html5.h>

extern libvlc_instance_t *libvlc;

// TODO - explain why we have to redefine everything

libvlc_media_player_t* EMSCRIPTEN_KEEPALIVE wasm_media_player_new() {
  return libvlc_media_player_new(libvlc);
}

libvlc_media_player_t* EMSCRIPTEN_KEEPALIVE wasm_media_player_new_from_media(libvlc_media_t* media) {
  return libvlc_media_player_new_from_media(media);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_release(libvlc_media_player_t *media_player) {
  libvlc_media_player_release(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasmc_media_player_retain(libvlc_media_player_t *media_player) {
  libvlc_media_player_retain(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_set_media(libvlc_media_player_t *media_player, libvlc_media_t *media) {
  libvlc_media_player_set_media(media_player, media);
}

libvlc_media_t* EMSCRIPTEN_KEEPALIVE wasm_media_player_get_media(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_media(media_player);
}

// TODO
// LIBVLC_API libvlc_event_manager_t * libvlc_media_player_event_manager (libvlc_media_player_t *p_mi );

EM_BOOL EMSCRIPTEN_KEEPALIVE wasm_media_player_is_playing(libvlc_media_player_t *media_player) {
  return libvlc_media_player_is_playing(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_play(libvlc_media_player_t *media_player) {
  return libvlc_media_player_play(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_set_pause(libvlc_media_player_t *media_player, int do_pause) {
  libvlc_media_player_set_pause(media_player, do_pause);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_pause(libvlc_media_player_t *media_player) {
  libvlc_media_player_pause(media_player);
}

// TODO
// LIBVLC_API int libvlc_media_player_stop_async ( libvlc_media_player_t *p_mi );


libvlc_time_t EMSCRIPTEN_KEEPALIVE wasm_media_player_get_length(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_length(media_player);
}

libvlc_time_t EMSCRIPTEN_KEEPALIVE wasm_media_player_get_time(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_time(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_set_time(libvlc_media_player_t *media_player, libvlc_time_t time, bool fast) {
  return libvlc_media_player_set_time(media_player, time, fast);
}

float EMSCRIPTEN_KEEPALIVE wasm_media_player_get_position(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_position(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_set_position(libvlc_media_player_t *media_player, float position, EM_BOOL fast) {
  return libvlc_media_player_set_position(media_player, position, fast);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_set_chapter(libvlc_media_player_t *media_player, int chapter) {
  libvlc_media_player_set_chapter(media_player, chapter);
}

float EMSCRIPTEN_KEEPALIVE wasm_media_player_get_chapter(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_chapter(media_player);
}

float EMSCRIPTEN_KEEPALIVE wasm_media_player_get_chapter_count(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_chapter_count(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_get_chapter_count_for_title(libvlc_media_player_t *media_player, int title) {
  return libvlc_media_player_get_chapter_count_for_title(media_player, title);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_set_title(libvlc_media_player_t *media_player, int title) {
  libvlc_media_player_set_title(media_player, title);
}

float EMSCRIPTEN_KEEPALIVE wasm_media_player_get_title(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_title(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_get_title_count( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_get_title_count(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_previous_chapter( libvlc_media_player_t *media_player ) {
  libvlc_media_player_previous_chapter(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_next_chapter( libvlc_media_player_t *media_player ) {
  libvlc_media_player_next_chapter(media_player);
}

float EMSCRIPTEN_KEEPALIVE wasm_media_player_get_rate( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_get_rate(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_set_rate( libvlc_media_player_t *media_player, float rate ) {
  return libvlc_media_player_set_rate(media_player, rate);
}


unsigned EMSCRIPTEN_KEEPALIVE wasm_media_player_has_vout( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_has_vout(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE wasm_media_player_is_seekable( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_is_seekable(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE wasm_media_player_can_pause( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_can_pause(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE wasm_media_player_program_scrambled( libvlc_media_player_t *media_player ) {
  return libvlc_media_player_program_scrambled(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_media_player_next_frame( libvlc_media_player_t *media_player ) {
  libvlc_media_player_next_frame(media_player);
}


int EMSCRIPTEN_KEEPALIVE wasm_video_get_size_x(libvlc_media_player_t *media_player, unsigned num) {
  unsigned x = 0;
  unsigned y = 0;
  int res = libvlc_video_get_size(media_player, num, &x, &y);
  if (res == -1)
    return -1;
  else
    return x;
}

int EMSCRIPTEN_KEEPALIVE wasm_video_get_size_y(libvlc_media_player_t *media_player, unsigned num) {
  unsigned x = 0;
  unsigned y = 0;
  int res = libvlc_video_get_size(media_player, num, &x, &y);
  if (res == -1)
    return -1;
  else
    return y;
}

int EMSCRIPTEN_KEEPALIVE wasm_video_get_cursor_x(libvlc_media_player_t *media_player, unsigned num) {
  int x = 0;
  int y = 0;
  int res = libvlc_video_get_cursor(media_player, num, &x, &y);
  if (res == -1)
    return -1;
  else
    return x;
}

int EMSCRIPTEN_KEEPALIVE wasm_video_get_cursor_y(libvlc_media_player_t *media_player, unsigned num) {
  int x = 0;
  int y = 0;
  int res = libvlc_video_get_cursor(media_player, num, &x, &y);
  if (res == -1)
    return -1;
  else
    return y;
}

void EMSCRIPTEN_KEEPALIVE wasm_audio_toggle_mute(libvlc_media_player_t *media_player) {
  libvlc_audio_toggle_mute(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE wasm_audio_get_mute(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_mute(media_player);
}

void EMSCRIPTEN_KEEPALIVE wasm_audio_set_mute(libvlc_media_player_t *media_player, int status) {
  libvlc_audio_set_mute(media_player, status);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_get_volume(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_volume(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_set_volume(libvlc_media_player_t *media_player, int volume) {
  return libvlc_audio_set_volume(media_player, volume);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_get_channel(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_channel(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_set_channel(libvlc_media_player_t *media_player, int channel) {
  return libvlc_audio_set_channel(media_player, channel);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_get_delay(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_delay(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_audio_set_delay(libvlc_media_player_t *media_player, int delay) {
  return libvlc_audio_set_delay(media_player, delay);
}


// TODO - Export libvlc_media_player_role constants

int EMSCRIPTEN_KEEPALIVE wasm_media_player_get_role(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_role(media_player);
}

int EMSCRIPTEN_KEEPALIVE wasm_media_player_set_role(libvlc_media_player_t *media_player, unsigned role) {
  return libvlc_media_player_set_role(media_player, role);
}
