#include <stdio.h>
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <assert.h>
#include <errno.h>

#include <emscripten.h>
#include <emscripten/html5.h>

libvlc_media_player_t *mp;
libvlc_instance_t *libvlc;
libvlc_time_t t = -1;

void pause_video(void* _arg);
void stop_video(void* _arg);

int main() {
	/* We don't want to the main thread stop even if the main function exit.
	 * If this thread stop, all proxyfied functions wont be called.
	 */
  EM_ASM(Module['noExitRuntime']=true);
  libvlc_media_t *m;
  char const *vlc_argv[] = {
    "-vvv",
    "--no-spu",
    "--no-osd",
    "--aout=emworklet_audio",
    "-Idummy",
    "--ignore-config",
  };

  libvlc = libvlc_new( ARRAY_SIZE( vlc_argv ), vlc_argv );
  assert(libvlc);
  m = libvlc_media_new_path( libvlc, "./BigBuckBunny.mp4" );
  assert(m);
  mp = libvlc_media_player_new_from_media(m);
  assert(mp);

  libvlc_media_release(m);
  m = libvlc_media_player_get_media(mp);

  int res = libvlc_media_player_play(mp);
  assert(res == 0);

  emscripten_async_call(pause_video, NULL, 2 * 1000);
  emscripten_async_call(stop_video, NULL, 3 * 1000);
  fprintf(stderr, "test: end of main\n");

  return 0;
}

void pause_video(void* _arg) {
  fprintf(stderr, "test: pause\n");
  libvlc_media_player_set_pause(mp, true);
}

void stop_video(void* _arg) {
  fprintf(stderr, "test: stop\n");
  int res = libvlc_media_player_stop_async(mp);
  assert(res == 0);
  libvlc_media_player_release( mp );
  libvlc_release( libvlc );
}
