#include <stdio.h>
#include <vlc/vlc.h>
#include <vlc_common.h>
#include <errno.h>

#include <emscripten.h>
#include <emscripten/html5.h>

typedef struct ctrl_sys_t {
    libvlc_time_t t;
    
    char flag; // todo: make it an enum INIT/PAUSED/PLAYING
    char init_flag;
    char **vlc_options;
    int opts_count;
} ctrl_sys_t;

static void iter(void *userData) {
    ctrl_sys_t* sys = (ctrl_sys_t *)userData;
    /*    
	if (libvlc_media_player_get_time(sys->mp) == sys->t) {
		// when enable, the js does not respond.
		//libvlc_media_player_release( mp );
    	//libvlc_release( libvlc );
		emscripten_cancel_main_loop();
	}
	sys->t = libvlc_media_player_get_time(sys->mp);
    */
}

static EM_BOOL play_pause_handler(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    (void) e;
    ctrl_sys_t* sys = (ctrl_sys_t *)userData;    

    if (eventType == EMSCRIPTEN_EVENT_CLICK) {
        if ( sys->flag == 1 ) {

            sys->flag = 0;
        }
    }
    return 0;
}

static EM_BOOL libvlc_init(int eventType, const EmscriptenMouseEvent *e, void *userData) {
    (void) e;
    libvlc_instance_t *libvlc;
    libvlc_media_t *m;
    libvlc_media_player_t *mp;
        
    ctrl_sys_t* sys = (ctrl_sys_t *)userData;
        
    if (eventType == EMSCRIPTEN_EVENT_CLICK) {
        if (sys->init_flag == 1) {
            const char *args[sys->opts_count];
            int count = 0;

            memcpy(args + count, sys->vlc_options + 1, (sys->opts_count - 1) * sizeof (*(sys->vlc_options)));
            count += (sys->opts_count - 1);
            args[count] = NULL;

            libvlc = libvlc_new( count,  args );
            if (libvlc == NULL) {
                fprintf( stderr, "unable to create libvlc instance" );
                return -1;
            }
            m = libvlc_media_new_path( libvlc, args[count-1]);
            
            if (m == NULL) {
                fprintf(stderr, "unable to create media");
                return -1;
            }
            mp = libvlc_media_player_new_from_media( m );
            if (mp == NULL) {
                fprintf(stderr, "unable to create media player");
                return -1;
            }
            
            libvlc_media_release( m );
            m = libvlc_media_player_get_media(mp);
            libvlc_media_player_play(mp);
            sys->init_flag = 0;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    EM_ASM(Module['noExitRuntime']= true);
    ctrl_sys_t* sys = (ctrl_sys_t *)malloc(sizeof(ctrl_sys_t));    

    sys->opts_count = argc; 
    sys->vlc_options = argv;
    sys->flag = 1;
    sys->init_flag = 1;
    // sys->mp = NULL; 
    
    emscripten_set_click_callback("#canvas", sys, 1, play_pause_handler);
    emscripten_set_click_callback("#libvlc_init", sys, 1, libvlc_init);

    emscripten_set_main_loop_arg(iter, sys, 0, 1);
    
    return 0;
}
