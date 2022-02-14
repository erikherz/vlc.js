#include <stdio.h>
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <assert.h>
#include <errno.h>

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/nativefs.h>

libvlc_media_player_t *mp;
libvlc_instance_t *libvlc;
libvlc_time_t t = -1;

static void iter()
{
  if (!mp)
    return;
	if (libvlc_media_player_get_time(mp) == t) {
		// when enable, the js does not respond.
		//libvlc_media_player_release( mp );
    	//libvlc_release( libvlc );
		emscripten_cancel_main_loop();
	}
	t = libvlc_media_player_get_time(mp);
}

void EMSCRIPTEN_KEEPALIVE set_global_media_player(libvlc_media_player_t *media_player) {
  mp = media_player;
}

extern void update_overlay();


static void on_position_changed(const libvlc_event_t *p_event, void *p_data){
  VLC_UNUSED(p_event);
  VLC_UNUSED(p_data);
  
  MAIN_THREAD_ASYNC_EM_ASM({
    const overlay = document.getElementById("overlay");
    update_overlay(overlay);
  });
}

int main() {

	/* We don't want to the main thread stop even if the main function exit.
	 * If this thread stop, all proxyfied functions wont be called.
	 */
  EM_ASM(Module['noExitRuntime']=true);
  nativefs_init(1, "#btn");
  char const *vlc_argv[] = {
			    "-vvv",
			    "--no-spu",
			    "--no-osd",
			    "--aout=emworklet_audio",
			    "-Idummy",
			    "--ignore-config",
  };

  libvlc = libvlc_new( ARRAY_SIZE( vlc_argv ), vlc_argv );
  if (libvlc == NULL)
    {
      fprintf( stderr, "unable to create libvlc instance" );
      return -1;
    }

  emscripten_set_main_loop(iter, 1, 1);

  return 0;
}

// Used to make sure the UI (progress bar, play/pause button, etc) is
// updated as the video is read.
void EMSCRIPTEN_KEEPALIVE attach_update_events(libvlc_media_player_t *media_player) {
  libvlc_event_manager_t* event_manager = libvlc_media_player_event_manager(media_player);
  int res;
  res = libvlc_event_attach(
    event_manager,
    libvlc_MediaPlayerPositionChanged,
    on_position_changed,
    NULL
  );
  assert(res == 0);
  res = libvlc_event_attach(
    event_manager,
    libvlc_MediaPlayerPaused,
    on_position_changed,
    NULL
  );
  assert(res == 0);
}
