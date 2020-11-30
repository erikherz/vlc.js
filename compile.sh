#!/bin/bash

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

apply_patch() {
    diagnostic "Applying $1..."
    git am -3 $1 || git am --skip
}


VLCJS_DIR=$(pwd)
PROJECT_DIR=$VLCJS_DIR/build
TESTED_HASH="4deb73b6f62ce8fe8dea84ab0b0a138b79af4550"
OFFSCREEN="OFFSCREEN_FRAMEBUFFER=1"

# download emscripten sdk
if [ ! -d emsdk ]; then
    diagnostic "emsdk not found. Fetching it"
    git clone http://github.com/emscripten-core/emsdk.git emsdk
    cd $VLCJS_DIR/emsdk && ./emsdk update-tags && ./emsdk install latest && ./emsdk activate latest
    checkfail "emsdk: fetch failed"
    diagnotsic "applying emsripten-fs patch"
    cd upstream/emscripten && \
	patch -p1 < $VLCJS_DIR/vlc_patches/emscripten-nativefs.patch
    checkfail "could not apply emscripten-fs patch"
    cd $VLCJS_DIR
    diagnostic "successfully downloaded and patched toolchain"
fi

. emsdk/emsdk_env.sh

# download vlc
if [ ! -d vlc ]; then
    diagnostic "VLC source not found, cloning"
    git clone http://git.videolan.org/git/vlc.git vlc 
    checkfail "vlc source: git clone failed"
    cd $VLCJS_DIR/vlc
    
    diagnostic "applying vlc patches"
    git reset --hard ${TESTED_HASH}
    checkfail "vlc.js: could not reset to ${TESTED_HASH}"
    
    apply_patch $VLCJS_DIR/vlc_patches/0001-nacl-remove-deprecated-platform.patch
    apply_patch $VLCJS_DIR/vlc_patches/0002-wasm-emscripten-Create-a-target-for-emscripten-in-th.patch
    apply_patch $VLCJS_DIR/vlc_patches/0003-compat-add-clock_nanosleep-sigwait-support-for-emscr.patch
    apply_patch $VLCJS_DIR/vlc_patches/0004-core-initial-core-build-for-emscripten-based-on-POSI.patch
    apply_patch $VLCJS_DIR/vlc_patches/0005-contrib-add-emscripten-support-for-openjpeg.patch
    apply_patch $VLCJS_DIR/vlc_patches/0006-contrib-add-ffmpeg-configuration-options-for-wasm-em.patch
    apply_patch $VLCJS_DIR/vlc_patches/0007-demux-disable-ytdl-module.patch
    apply_patch $VLCJS_DIR/vlc_patches/0008-logger-add-emscripten-module.patch
    apply_patch $VLCJS_DIR/vlc_patches/0009-aout-add-an-openal-audio-module.patch
    apply_patch $VLCJS_DIR/vlc_patches/0010-aout-add-audio-worklet-support.patch
    apply_patch $VLCJS_DIR/vlc_patches/0011-vout-add-emscripten-gl-es2-module.patch
    apply_patch $VLCJS_DIR/vlc_patches/0015-extras-add-wasm-emscripten-build-script.patch

    #wip:
    apply_patch $VLCJS_DIR/vlc_patches/0016-fixup-add-.bc-suffix-for-module-compilation.patch
    apply_patch $VLCJS_DIR/vlc_patches/0017-fixup-temporarily-disable-dav1d.patch
    checkfail "vlc.js: there was an issue while applying patches"
    cd $VLCJS_DIR
fi

# TODO: patches for emscriptenfs + offscreen_canvas
#apply_patch 0012-vout-add-offscreen-canvas-initial-support.patch
#apply_patch 0015-access-initial-emscripten-file-api-support.patch

#build libvlc
# TODO: add this in the script if the triplet is not supported
# cp ../config.sub vlc/autotools/config.sub
. $VLCJS_DIR/vlc/extras/package/wasm-emscripten/build.sh
cp -r $VLCJS_DIR/vlc/include $VLCJS_DIR/build/.
cd $VLCJS_DIR

url="http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4"
#download media
if [ ! -f BigBuckBunny.mp4 ]; then
    diagnostic "Downloading mediafile..."
    curl ${url} -o BigBuckBunny.mp4
    cp BigBuckBunny.mp4 $VLCJS_DIR/build/build-emscripten/.
    checkfail "failed to download mediafile!"
    diagnostic "Success: mediafile downloaded"
fi

# ci build procedure + cp vlc/include
diagnostic "vlc.js: build vlc.js"
cd $VLCJS_DIR/build/build-emscripten
# for release: add -Os
# for source maps to work, the preloaded file shouldn't be too heavy
# if not serving locally localhost:6931 should be changed

# if you can if you want to use the sanitizer add:
# -fsanitize=address -s ALLOW_MEMORY_GROWTH=1
emcc -s USE_PTHREADS=1 -s TOTAL_MEMORY=1GB  \
     -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 \
     -s ${OFFSCREEN} --profiling-funcs \
     -s ASSERTIONS=2 -s SAFE_HEAP=1 -s STACK_OVERFLOW_CHECK=1 \
     -g4 --source-map-base http://localhost:6931/ \
     -I $PROJECT_DIR/include/ -I $PROJECT_DIR/wasm32-unknown-emscripten/include/ $VLCJS_DIR/main.c \
     $PROJECT_DIR/build-emscripten/lib/.libs/libvlc.a \
     $PROJECT_DIR/build-emscripten/vlc-modules.bc $PROJECT_DIR/build-emscripten/modules/.libs/*.a \
     $PROJECT_DIR/wasm32-unknown-emscripten/lib/*.a \
     $PROJECT_DIR/build-emscripten/src/.libs/libvlccore.a \
     $PROJECT_DIR/build-emscripten/compat/.libs/libcompat.a \
     -o $VLCJS_DIR/experimental.js --bind --emrun --preload-file $VLCJS_DIR/build/build-emscripten/BigBuckBunny.mp4

checkfail "could not compile vlc.js"
cd $VLCJS_DIR
diagnostic "successfully compiled vlc.js"

diagnostic "creating archives"

tar -cvjSf $VLCJS_DIR/vlc-contrib-alpha.tar.bz2 $VLCJS_DIR/build/wasm32-unknown-emscripten
checkfail "error: could not create contrib archive"

zip -r build.zip assets/* experimental.* vlc.html
checkfail "error: could not create build archive"

diagnostic "successfully created archives"

emrun --no_browser vlc.html
diagnostic "vlc.js: serving demo"
