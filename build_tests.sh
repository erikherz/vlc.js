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
SAMPLE=${SAMPLE:=./BigBuckBunny.mp4}
PROJECT_DIR=${PROJECT_DIR:=./vlc/extras/package/wasm-emscripten/build}

mkdir -p tests/build
cp vlc.html tests/build

emcc --bind -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB -s PTHREAD_POOL_SIZE=15 \
    -s OFFSCREEN_FRAMEBUFFER=1 -s USE_WEBGL2=1 --profiling-funcs \
    -I $PATH_VLC/include/ -I $PROJECT_DIR/wasm32-unknown-emscripten/include/ \
    tests/simple_test.c \
    $PROJECT_DIR/build-emscripten/lib/.libs/libvlc.a \
    $PROJECT_DIR/build-emscripten/vlc-modules.bc \
    $PROJECT_DIR/build-emscripten/modules/.libs/*.a \
    $PROJECT_DIR/wasm32-unknown-emscripten/lib/*.a \
    $PROJECT_DIR/build-emscripten/src/.libs/libvlccore.a \
    $PROJECT_DIR/build-emscripten/compat/.libs/libcompat.a \
    -o tests/build/experimental.html --preload-file ${SAMPLE}
