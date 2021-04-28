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

static void iter()
{
	if (libvlc_media_player_get_time(mp) == t) {
		// when enable, the js does not respond.
		//libvlc_media_player_release( mp );
    	//libvlc_release( libvlc );
		emscripten_cancel_main_loop();
	}
	t = libvlc_media_player_get_time(mp);
}

extern void update_overlay();
extern void on_position_changed(const libvlc_event_t *p_event, void *p_data);

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

  if (libvlc == NULL)
    {
      fprintf( stderr, "unable to create libvlc instance" );
      return -1;
    }
  m = libvlc_media_new_path( libvlc, "./samples/BigBuckBunny.mp4" );

  if (m == NULL)
    {
      fprintf(stderr, "unable to create media");
      return -1;
    }
  mp = libvlc_media_player_new_from_media( m );
  if (mp == NULL)
    {
      fprintf(stderr, "unable to create media player");
      return -1;
    }

  libvlc_media_release( m );
  m = libvlc_media_player_get_media(mp);

  libvlc_event_manager_t* event_manager = libvlc_media_player_event_manager(mp);
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

  emscripten_set_main_loop(iter, 1, 1);

  return 0;
}


int g_is_started = 0;

void EMSCRIPTEN_KEEPALIVE play_video() {
    if (g_is_started == 0)
    {
      libvlc_media_player_play(mp);
      g_is_started = 1;
    }
    else {
      libvlc_media_player_pause(mp);
    }
    update_overlay();
}

EM_BOOL EMSCRIPTEN_KEEPALIVE is_paused() {
  return !libvlc_media_player_is_playing(mp);
}

float EMSCRIPTEN_KEEPALIVE get_position() {
  return libvlc_media_player_get_position(mp);
}

int EMSCRIPTEN_KEEPALIVE set_position(float position) {
  // TODO - what does "fast" argument do?
  return libvlc_media_player_set_position(mp, position, false);
}

int EMSCRIPTEN_KEEPALIVE get_volume() {
  return libvlc_audio_get_volume(mp);
}

int EMSCRIPTEN_KEEPALIVE set_volume(int i_volume) {
  return libvlc_audio_set_volume(mp, i_volume);
}

void EMSCRIPTEN_KEEPALIVE toggle_mute() {
  libvlc_audio_toggle_mute(mp);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE get_mute() {
  return libvlc_audio_get_mute(mp);
}

void EMSCRIPTEN_KEEPALIVE set_mute(int i_status) {
  libvlc_audio_set_mute(mp, i_status);
}
