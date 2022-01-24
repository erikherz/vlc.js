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
emcc --bind -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB -s PTHREAD_POOL_SIZE=15 \
    -s OFFSCREEN_FRAMEBUFFER=1 -s USE_WEBGL2=1 --profiling-funcs \
    -s MODULARIZE=1 -s EXPORT_NAME="VlcModule" \
    -s EXPORTED_RUNTIME_METHODS="allocateUTF8" \
    -I $PATH_VLC/include/ \
    main.c exports_media_player.c exports_media.c \
    $PATH_VLC/build-emscripten/lib/.libs/libvlc.a \
    $PATH_VLC/build-emscripten/vlc-modules.bc \
    $PATH_VLC/build-emscripten/modules/.libs/*.a \
    $PATH_VLC/contrib/wasm32-unknown-emscripten/lib/*.a \
    $PATH_VLC/build-emscripten/src/.libs/libvlccore.a \
    $PATH_VLC/build-emscripten/compat/.libs/libcompat.a \
    --js-library lib/wasm-imports.js \
    --js-library vlc/modules/audio_output/webaudio/webaudio.js \
    -o experimental.js --preload-file ${SAMPLE_DIR}
