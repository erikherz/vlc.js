#include <stdio.h>
#include <vlc/vlc.h>
#include <assert.h>
#include <errno.h>

#define restrict

#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <vlc_common.h>
#include <vlc_vout_window.h>
#include <vlc_opengl.h>
#include <vlc_vout_display.h>
#include <vlc_opengl_filter.h>
#include <vlc_opengl_interop.h>
#include <vlc_modules.h>
#include <vlc_messages.h>

#include "vlc/lib/libvlc_internal.h"

struct test_scenario;
static void scenario1_clear(struct test_scenario *scenario);
static void scenario3_clearsmooth(struct test_scenario *scenario);
static void scenario4_mock(struct test_scenario *scenario);
static void scenario5_videoframe(struct test_scenario *scenario);
static void scenario7_display(struct test_scenario *scenario);

struct webcodec_context
{
    pthread_t gl_worker;
};

libvlc_instance_t *libvlc;

struct test_scenario {
    const char *canvas;
    void (*setup)(struct test_scenario *scenario);
    float color[3];

    vout_window_t *window;
    vlc_gl_t *gl;
    vlc_thread_t thread;
} tests[] =  {
    {
        .canvas = "canvas_step1",
        .setup = scenario1_clear,
        .color = { 1.f, 0.f, 0.f },
    },
    {
        .canvas = "canvas_step2",
        .setup = scenario1_clear,
        .color = { 0.f, 1.f, 0.f },
    },
    {
        .canvas = "canvas_step3",
        .setup = scenario3_clearsmooth,
    },
    {
        .canvas = "canvas_step4",
        .setup = scenario4_mock,
    },
    {
        .canvas = "canvas_step5",
        .setup = scenario5_videoframe,
    },
    {
        .canvas = "canvas_step7",
        .setup = scenario7_display,
    }

};

#define DECLARE_MEMBER(type, name) type name;
#define LOAD_SYMBOL(type, name) \
    gl.name = (type)vlc_gl_GetProcAddress(scenario->gl, "gl" #name); \
    assert(gl.name);

#define CREATE_VTABLE(VTABLE) \
    struct { \
        VTABLE(DECLARE_MEMBER) \
    } gl; \
    VTABLE(LOAD_SYMBOL)

static void iter()
{
    return;
}

static void *test_run(void *opaque)
{
    auto *test = static_cast<struct test_scenario *>(opaque);
    libvlc_int_t *root = libvlc->p_libvlc_int;

    test->window = vlc_object_create<vout_window_t>(root);
    test->window->handle.canvas = test->canvas;
    test->window->type = VOUT_WINDOW_TYPE_EMSCRIPTEN_WEBGL;
    struct vout_display_cfg cfg = {
        .window = test->window,
    };
    test->setup(test);

    return NULL;
}

static void init_gl(struct test_scenario *scenario)
{
    struct vout_display_cfg cfg = {
        .window = scenario->window,
    };

    scenario->gl = vlc_gl_Create(&cfg, VLC_OPENGL_ES2, "any");
    assert(scenario->gl);
}

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

    emscripten_log(EM_LOG_INFO | EM_LOG_CONSOLE, "Creating tests...\n");
    for (size_t i=0; i<ARRAY_SIZE(tests); ++i)
    {
        emscripten_log(EM_LOG_INFO | EM_LOG_CONSOLE, "- test %zu/%zu...\n",
            i, ARRAY_SIZE(tests));
        vlc_clone(&tests[i].thread, test_run, &tests[i], 0);
    }

    emscripten_set_main_loop(iter, 1, 1);

    return 0;
}

static void scenario1_clear(struct test_scenario *scenario)
{
    init_gl(scenario);

    vlc_gl_MakeCurrent(scenario->gl);

#define OPENGL_VTABLE_F(F) \
    F(PFNGLCLEARCOLORPROC, ClearColor) \
    F(PFNGLCLEARPROC, Clear)
    CREATE_VTABLE(OPENGL_VTABLE_F);
#undef OPENGL_VTABLE_F

    gl.ClearColor(scenario->color[0], scenario->color[1], scenario->color[2], 1.f);
    gl.Clear(GL_COLOR_BUFFER_BIT);
    vlc_gl_Swap(scenario->gl);
    vlc_gl_ReleaseCurrent(scenario->gl);
}

static void ClearColorAtTime(void *opaque)
{
    auto *scenario = static_cast<struct test_scenario *>(opaque);
    vlc_gl_MakeCurrent(scenario->gl);

#define OPENGL_VTABLE_F(F) \
    F(PFNGLCLEARCOLORPROC, ClearColor) \
    F(PFNGLCLEARPROC, Clear)
    CREATE_VTABLE(OPENGL_VTABLE_F);
#undef OPENGL_VTABLE_F

    vlc_tick_t now = vlc_tick_now();
    float secf = secf_from_vlc_tick(now);
    secf = fmod(secf / 5.f, 1.f);

    gl.ClearColor(secf, 0.f, 0.f, 1.f);
    gl.Clear(GL_COLOR_BUFFER_BIT);
    vlc_gl_Swap(scenario->gl);
    vlc_gl_ReleaseCurrent(scenario->gl);

}

static void scenario3_clearsmooth(struct test_scenario *scenario)
{
    init_gl(scenario);

    emscripten_set_main_loop_arg(ClearColorAtTime, scenario, 0, false);
}

static void scenario4_mock(struct test_scenario *scenario)
{
    init_gl(scenario);

    vlc_gl_MakeCurrent(scenario->gl);

#define OPENGL_VTABLE_F(F) \
    F(PFNGLCLEARCOLORPROC, ClearColor) \
    F(PFNGLCLEARPROC, Clear)
    CREATE_VTABLE(OPENGL_VTABLE_F);
#undef OPENGL_VTABLE_F

    auto end = [&]{
        gl.ClearColor(0.f, 1.f, 0.f, 1.f);
        gl.Clear(GL_COLOR_BUFFER_BIT);
        vlc_gl_Swap(scenario->gl);
        vlc_gl_ReleaseCurrent(scenario->gl);
    };

    size_t count_strict;
    module_t **modules;
    ssize_t count = vlc_module_match(
            "opengl filter", "mock", false,
            &modules, &count_strict);


    if (count == 0)
    {
        msg_Err(scenario->gl, "No opengl filter module named mock found");
        end();
        return;
    }

    gl.ClearColor(0.f, 0.f, 0.f, 1.f);
    gl.Clear(GL_COLOR_BUFFER_BIT);


    struct vlc_gl_input_meta meta = {
        .pts = VLC_TICK_0,
    };
    struct vlc_gl_tex_size texsize = { .width = 400, .height = 300 };
    auto filter = vlc_object_create<struct vlc_gl_filter>(scenario->gl);
    filter->gl = scenario->gl;

    auto *activate = reinterpret_cast<vlc_gl_filter_open_fn*>(
            vlc_module_map(vlc_object_logger(scenario->gl), modules[0]));

    if (activate(filter, NULL, NULL, &texsize) != VLC_SUCCESS)
    {
        end();
        return;
    }

    filter->ops->draw(filter, NULL, &meta);

    vlc_gl_Swap(scenario->gl);
    vlc_gl_ReleaseCurrent(scenario->gl);
    return;
}

static int InteropInit(struct vlc_gl_interop *interop, uint32_t tex_target,
        vlc_fourcc_t chroma, video_color_space_t yuv_space)
{
    return VLC_SUCCESS;
}

EM_JS(void, SendVideoFrame, (int worker_id), {
    let buffer = new Uint8Array(4*32*32);
    for (let i=0; i<32; ++i)
    {
        for (let j=0; j<32; ++j)
        {
            let r1 = i - 16;
            let r2 = j - 16;
            let value = 255;
            /* Let's draw a disk to recognize shape and size*/
            if (r1*r1 + r2*r2 > 10 * 10)
                value = 0;
            buffer[4*(i*32 + j) + 0] = value;
            buffer[4*(i*32 + j) + 1] = value;
            buffer[4*(i*32 + j) + 2] = value;
            buffer[4*(i*32 + j) + 3] = 255;
        }
    }
    let frame = new VideoFrame(buffer, {
        format: 'RGBA',
        codedWidth: 32,
        codedHeight: 32,
        displayWidth: 32,
        displayHeight: 32,
        timestamp: 0,
    });
    self.postMessage({
        cmd: 'custom',
        customCmd: 'displayFrame',
        frame: frame,
        targetThread: worker_id,
        pictureId: 0,
    }, [frame]);
    frame.close();
})

static void scenario5_videoframe(struct test_scenario *scenario)
{
    init_gl(scenario);

    vlc_gl_MakeCurrent(scenario->gl);

#define OPENGL_VTABLE_F(F) \
    F(PFNGLCLEARCOLORPROC, ClearColor) \
    F(PFNGLCLEARPROC, Clear) \
    F(PFNGLBINDTEXTUREPROC, BindTexture) \
    F(PFNGLGENTEXTURESPROC, GenTextures) \
    F(PFNGLGENFRAMEBUFFERSPROC, GenFramebuffers) \
    F(PFNGLBINDFRAMEBUFFERPROC, BindFramebuffer) \
    F(PFNGLFRAMEBUFFERTEXTURE2DPROC, FramebufferTexture2D) \
    F(PFNGLBLITFRAMEBUFFERPROC, BlitFramebuffer)
    CREATE_VTABLE(OPENGL_VTABLE_F);
#undef OPENGL_VTABLE_F

    auto end = [&]{
        gl.ClearColor(0.f, 1.f, 0.f, 1.f);
        gl.Clear(GL_COLOR_BUFFER_BIT);
        vlc_gl_Swap(scenario->gl);
        vlc_gl_ReleaseCurrent(scenario->gl);
    };

    struct vlc_video_context *vctx =
        vlc_video_context_Create(NULL, VLC_VIDEO_CONTEXT_WEBCODEC,
                                 sizeof(struct webcodec_context), NULL);
    auto *vctxPrivate = static_cast<struct webcodec_context *>(
            vlc_video_context_GetPrivate(vctx, VLC_VIDEO_CONTEXT_WEBCODEC));

    auto interop = vlc_object_create<struct vlc_gl_interop>(scenario->gl);
    interop->gl = scenario->gl;
    interop->vctx = vctx;
    interop->init = InteropInit;
    video_format_Init(&interop->fmt_in, VLC_CODEC_WEBCODEC_OPAQUE);
    interop->fmt_in.i_visible_width
        = interop->fmt_in.i_width
        = 32;
    interop->fmt_in.i_visible_height
        = interop->fmt_in.i_height
        = 32;
    interop->tex_target = GL_TEXTURE_2D;

    interop->module = module_need(interop, "glinterop", "any", false);
    if (interop->module == NULL)
    {
        end();
        return;
    }

    gl.ClearColor(0.f, 0.f, 0.f, 1.f);
    gl.Clear(GL_COLOR_BUFFER_BIT);

    SendVideoFrame(vctxPrivate->gl_worker);

    picture_t *pic = picture_NewFromFormat(&interop->fmt_in);
    assert(pic);
    GLsizei tex_width, tex_height;

    GLuint texture;
    gl.GenTextures(1, &texture);
    interop->ops->update_textures(interop, &texture, &tex_width, &tex_height,
                                  pic, NULL);

    //GLuint defaultFb;
    //gl.GetIntergerv(GL_FRAMEBUFFER_BINDING, &defaultFb);

    GLuint fb;
    gl.GenFramebuffers(1, &fb);
    gl.BindFramebuffer(GL_READ_FRAMEBUFFER, fb);
    gl.FramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    gl.BlitFramebuffer(0, 0, 32, 32,
                       0, 0, 200, 200,
                       GL_COLOR_BUFFER_BIT,
                       GL_NEAREST);

    vlc_gl_Swap(scenario->gl);
    vlc_gl_ReleaseCurrent(scenario->gl);
    return;
}

void scenario7_display(struct test_scenario *scenario)
{
    struct vout_display_cfg cfg = {
        .window = scenario->window,
        .display = { .width=400, .height=300, .sar = {1,1} },
        .is_display_filled = true,
    };

    video_format_t format;
    video_format_Init(&format, VLC_CODEC_WEBCODEC_OPAQUE);
    format.i_visible_width
        = format.i_width
        = 32;
    format.i_visible_height
        = format.i_height
        = 32;
    format.i_sar_num = format.i_sar_den = 1;

    struct vlc_video_context *vctx =
        vlc_video_context_Create(NULL, VLC_VIDEO_CONTEXT_WEBCODEC,
                                 sizeof(struct webcodec_context), NULL);
    auto *vctxPrivate = static_cast<struct webcodec_context *>(
            vlc_video_context_GetPrivate(vctx, VLC_VIDEO_CONTEXT_WEBCODEC));

    vout_display_t *vd = vout_display_New(scenario->window, &format, vctx, &cfg, "gles2", NULL);

    SendVideoFrame(vctxPrivate->gl_worker);
    picture_t *pic = picture_NewFromFormat(&format);
    assert(pic);

    vout_display_Prepare(vd, pic, NULL, VLC_TICK_0);
    vout_display_Display(vd, pic);
}
