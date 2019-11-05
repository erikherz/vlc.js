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

PROJECT_DIR=$(pwd)/vlc

cd vlc/build-emscripten
# for release, remove profiling-funcs and add -Os
emcc -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB  \
    -s OFFSCREEN_FRAMEBUFFER=1 --profiling-funcs \
    -I $PROJECT_DIR/include/ -I $PROJECT_DIR/contrib/wasm32-unknown-emscripten/include/ main.c \
    $PROJECT_DIR/build-emscripten/lib/.libs/libvlc.a \
    vlc-modules.bc $PROJECT_DIR/build-emscripten/modules/.libs/*.a \
    $PROJECT_DIR/contrib/wasm32-unknown-emscripten/lib/*.a \
    $PROJECT_DIR/build-emscripten/src/.libs/libvlccore.a \
    $PROJECT_DIR/build-emscripten/compat/.libs/libcompat.a \
    -o experimental.html --emrun --preload-file BigBuckBunny.mp4
