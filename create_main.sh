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

# Theses flags are to use for webgl2 (see : https://github.com/kripken/emscripten/issues/5437)
# -s USE_WEBGL2=1 -s FULL_ES3=1
emcc -O2 -s USE_PTHREADS=1 -s TOTAL_MEMORY=134217728  \
    -s PTHREAD_POOL_SIZE=5 -s ASSERTIONS=2 -s STACK_OVERFLOW_CHECK=2 \
    -s FULL_ES2=1 -s OFFSCREENCANVAS_SUPPORT=1 -s GL_ASSERTIONS=1 -s GL_DEBUG=1 \
    -s OPENAL_DEBUG=1 \
    -I $PROJECT_DIR/include/ -I $PROJECT_DIR/contrib/asmjs-unknown-emscripten/include/ main.c \
    $PROJECT_DIR/build-emscripten/lib/.libs/libvlc.a \
    vlc-modules.bc $PROJECT_DIR/build-emscripten/modules/.libs/*.a \
    $PROJECT_DIR/contrib/asmjs-unknown-emscripten/lib/*.a \
    $PROJECT_DIR/build-emscripten/src/.libs/libvlccore.a \
    $PROJECT_DIR/build-emscripten/compat/.libs/libcompat.a \
    -o experimental.html --separate-asm --emrun --preload-file Dolby_Canyon.vob
