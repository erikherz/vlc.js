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
PROJECT_DIR=${PROJECT_DIR:=./vlc/extras/package/wasm-emscripten/build}

# for release, remove profiling-funcs and add -Os
emcc --bind -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB -s PTHREAD_POOL_SIZE=15 \
    -s OFFSCREEN_FRAMEBUFFER=1 -s USE_WEBGL2=1 --profiling-funcs \
    -s MODULARIZE=1 -s EXPORT_NAME="VlcModule" \
    -I $PATH_VLC/include/ -I $PROJECT_DIR/wasm32-unknown-emscripten/include/ main.c \
    $PROJECT_DIR/build-emscripten/lib/.libs/libvlc.a \
    $PROJECT_DIR/build-emscripten/vlc-modules.bc \
    $PROJECT_DIR/build-emscripten/modules/.libs/*.a \
    $PROJECT_DIR/wasm32-unknown-emscripten/lib/*.a \
    $PROJECT_DIR/build-emscripten/src/.libs/libvlccore.a \
    $PROJECT_DIR/build-emscripten/compat/.libs/libcompat.a \
     --js-library lib/wasm-imports.js \
    -o experimental.js --preload-file ${SAMPLE_DIR}
