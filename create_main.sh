#!/bin/bash
set -e

## FUNCTIONS

diagnostic()
{
     echo "$@" 1>&2;
}

checkfail()
{
    if [ ! $? -eq 0 ];then
        diagnostic "$1"
        exit 1
    fi
}

source emsdk/emsdk_env.sh

if [ ! -d vlc ]; then
    diagnostic "vlc must exists. Execute compile.sh"
    exit 1
fi

PATH_VLC=${PATH_VLC:=./vlc}
SAMPLE_DIR=${SAMPLE_DIR:=./samples}

# For release builds, remove '--profiling-funcs' and add '-Os'
# Note that we use '-s MODULARIZE', but no '-s EXPORT_ES6', which would
# conflict with pthreads on Firefox.

#    -s TRACE_WEBGL_CALLS=1 \

# -s ASYNCIFY_IMPORTS="['init_js_file', 'getVoutMessagePort', 'bindVideoFrame', 'CopyFrameToBuffer', 'probeConfig', 'initDecoderWorkerMessagePort', 'flushAsync', 'initDecoderJS']"

VLC_USE_SANITIZER=
DEBUG_OPTIONS=
while test -n "$1"
do
    case "$1" in
        --with-sanitizer=*)
            VLC_USE_SANITIZER="${1#--with-sanitizer=}"
            ;;
        --export-debug-symbols)
            DEBUG_OPTIONS="-g"
            ;;
        *)
            echo "Unrecognized options $1"
            exit 1
            ;;
    esac
    shift
done

SANITIZERS=
if echo "${VLC_USE_SANITIZER}" | grep address > /dev/null; then
SANITIZERS="$SANITIZERS -fsanitize=address -fsanitize-address-use-after-scope -fno-omit-frame-pointer"
fi

emcc --bind -s USE_PTHREADS=1 -s TOTAL_MEMORY=2GB -s PTHREAD_POOL_SIZE=25 \
    -s OFFSCREEN_FRAMEBUFFER=1  \
    -s USE_WEBGL2=1 \
    --profiling-funcs \
    -s OFFSCREENCANVAS_SUPPORT=1 \
    -s MODULARIZE=1 -s EXPORT_NAME="initModule" \
    -s EXPORTED_RUNTIME_METHODS="[allocateUTF8, writeAsciiToMemory]" \
    -s ASYNCIFY=1 -O3 \
    -s EXIT_RUNTIME=1 -s ASSERTIONS=1 \
    -I $PATH_VLC/include/ \
    main.c exports_media_player.c exports_media.c \
    -s EXPORTED_FUNCTIONS=@libvlc_wasm.sym \
    $DEBUG_OPTIONS \
    $SANITIZERS \
    $PATH_VLC/build-emscripten/lib/.libs/libvlc.a \
    $PATH_VLC/build-emscripten/vlc-modules.bc \
    $PATH_VLC/build-emscripten/modules/.libs/*.a \
    $PATH_VLC/contrib/wasm32-unknown-emscripten/lib/*.a \
    $PATH_VLC/build-emscripten/src/.libs/libvlccore.a \
    $PATH_VLC/build-emscripten/compat/.libs/libcompat.a \
    --js-library lib/wasm-imports.js \
    --js-library vlc/modules/audio_output/webaudio/webaudio.js \
    -o experimental.js

#em++ --bind -s USE_PTHREADS=1 -s TOTAL_MEMORY=2GB -s PTHREAD_POOL_SIZE=21 \
#    -s OFFSCREEN_FRAMEBUFFER=1\
#    -s USE_WEBGL2=1 \
#    --profiling-funcs \
#    $DEBUG_OPTIONS \
#    -s OFFSCREENCANVAS_SUPPORT=1 \
#    -s MODULARIZE=1 -s EXPORT_NAME="initModule" \
#    -s EXTRA_EXPORTED_RUNTIME_METHODS="[allocateUTF8]" \
#    -s ASYNCIFY=1 -O3 \
#    -s GL_ASSERTIONS=1 \
#    -s GL_TRACK_ERRORS=1 \
#    -I $PATH_VLC/include/ \
#    main_opengl.cpp \
#    $PATH_VLC/build-emscripten/lib/.libs/libvlc.a \
#    $PATH_VLC/build-emscripten/vlc-modules.bc \
#    $PATH_VLC/build-emscripten/modules/.libs/*.a \
#    $PATH_VLC/contrib/wasm32-unknown-emscripten/lib/*.a \
#    $PATH_VLC/build-emscripten/src/.libs/libvlccore.a \
#    $PATH_VLC/build-emscripten/compat/.libs/libcompat.a \
#    --js-library lib/wasm-imports.js \
#    -o opengl.js --preload-file ${SAMPLE_DIR}
