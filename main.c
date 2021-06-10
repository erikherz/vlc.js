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

void* EMSCRIPTEN_KEEPALIVE get_media_player(const char* path) {
  libvlc_media_player_t *media_player;

  libvlc_media_t *m;
  m = libvlc_media_new_path( libvlc, path );

  if (m == NULL)
    {
      fprintf(stderr, "unable to create media");
      return NULL;
    }
  media_player = libvlc_media_player_new_from_media( m );
  if (media_player == NULL)
    {
      fprintf(stderr, "unable to create media player");
      return NULL;
    }

  libvlc_media_release( m );
  m = libvlc_media_player_get_media(media_player);

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

  assert(media_player != NULL);
  mp = media_player;

  return media_player;
}


int EMSCRIPTEN_KEEPALIVE play(libvlc_media_player_t *media_player) {
  return libvlc_media_player_play(media_player);
}

void EMSCRIPTEN_KEEPALIVE pause_video(libvlc_media_player_t *media_player) {
  libvlc_media_player_pause(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE is_playing(libvlc_media_player_t *media_player) {
  return libvlc_media_player_is_playing(media_player);
}

float EMSCRIPTEN_KEEPALIVE get_position(libvlc_media_player_t *media_player) {
  return libvlc_media_player_get_position(media_player);
}

int EMSCRIPTEN_KEEPALIVE set_position(libvlc_media_player_t *media_player, float position, EM_BOOL fast) {
  return libvlc_media_player_set_position(media_player, position, fast);
}

int EMSCRIPTEN_KEEPALIVE get_volume(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_volume(media_player);
}

int EMSCRIPTEN_KEEPALIVE set_volume(libvlc_media_player_t *media_player, int i_volume) {
  return libvlc_audio_set_volume(media_player, i_volume);
}

void EMSCRIPTEN_KEEPALIVE toggle_mute(libvlc_media_player_t *media_player) {
  libvlc_audio_toggle_mute(media_player);
}

EM_BOOL EMSCRIPTEN_KEEPALIVE get_mute(libvlc_media_player_t *media_player) {
  return libvlc_audio_get_mute(media_player);
}

void EMSCRIPTEN_KEEPALIVE set_mute(libvlc_media_player_t *media_player, int i_status) {
  libvlc_audio_set_mute(media_player, i_status);
}
