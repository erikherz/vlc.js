#!/bin/sh
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

SLOW_MODE=${SLOW_MODE:=1}
WORK_DIR=$PWD

EMSDK_VERSION="3.1.18"
# Download the portable SDK and uncompress it
if [ ! -d emsdk ]; then
    diagnostic "emsdk not found. Fetching it"
    git clone http://github.com/emscripten-core/emsdk.git emsdk
    cd emsdk && ./emsdk update-tags && ./emsdk install ${EMSDK_VERSION} && ./emsdk activate ${EMSDK_VERSION}
    checkfail "emsdk: fetch failed"
fi

cd $WORK_DIR
TESTED_HASH="06e361b127e4609e429909756212ed5e30e7d032"
# Go go go vlc
if [ ! -d vlc ]; then
    diagnostic "VLC source not found, cloning"
    git clone https://code.videolan.org/videolan/vlc.git vlc || checkfail "VLC source: git clone failed"
    cd vlc
    diagnostic "VLC source: resetting to the TESTED_HASH commit (${TESTED_HASH})"
    git reset --hard ${TESTED_HASH} || checkfail "VLC source: TESTED_HASH ${TESTED_HASH} not found"
    # patching vlc
    if [ -d ../vlc_patches ] && [ "$(ls -A ../vlc_patches)" ]; then
	# core patches
	#git am -3 ../vlc_patches/0001-configure-improve-testing-unsupported-GL-functions-f.patch
	#git am -3 ../vlc_patches/0001-modules-disable-libvlc_json-and-ytbdl-vlc.js-17.patch
	#git am -3 ../vlc_patches/nacl-wasm/00*.patch
	#git am -3 ../vlc_patches/audio_output/00*.patch
	#git am -3 ../vlc_patches/video_output/00*.patch
	#git am -3 ../vlc_patches/logger/00*.patch
	#git am -3 ../vlc_patches/0001-vlc.js-modules-remove-category.patch
	#git am -3 ../vlc_patches/audio_output/new_aout.patch
	#git am -3 ../vlc_patches/filesystem/*.patch
	#git am -3 ../vlc_patches/demo_alpha/*
	git am -3  ../vlc_patches/aug/*
    fi
    checkfail "vlc source: git clone failed"
fi

cd $WORK_DIR
diagnostic "Setting the environment"
. emsdk/emsdk_env.sh

diagnostic "build libvlc"
cd ./vlc/extras/package/wasm-emscripten/
./build.sh --mode=${SLOW_MODE}
cd $WORK_DIR
echo "_main" > libvlc_wasm.sym
sed -e 's/^/_/' ./vlc/lib/libvlc.sym >> libvlc_wasm.sym


cd $WORK_DIR
diagnostic "Generating executable"
./create_main.sh
