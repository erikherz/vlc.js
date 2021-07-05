#! /bin/sh
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

EMSDK_VERSION="2.0.25"
# Download the portable SDK and uncompress it
if [ ! -d emsdk ]; then
    diagnostic "emsdk not found. Fetching it"
    git clone http://github.com/emscripten-core/emsdk.git emsdk
    cd emsdk && ./emsdk update-tags && ./emsdk install ${EMSDK_VERSION} && ./emsdk activate ${EMSDK_VERSION}
    checkfail "emsdk: fetch failed"
fi

cd $WORK_DIR
TESTED_HASH="721cf129"
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
	git am -3 ../vlc_patches/0001-configure-improve-testing-unsupported-GL-functions-f.patch
	git am -3 ../vlc_patches/0001-contrib-set-RANLIB-for-toolchain.cmake.patch
	git am -3 ../vlc_patches/0001-modules-disable-libvlc_json-and-ytbdl-vlc.js-17.patch
	git am -3 ../vlc_patches/nacl-wasm/00*.patch
	git am -3 ../vlc_patches/audio_output/00*.patch
	git am -3 ../vlc_patches/video_output/00*.patch
	git am -3 ../vlc_patches/video_output/offscreen-canvas.patch
	git am -3 ../vlc_patches/logger/00*.patch
	# git am -3 ../vlc_patches/filesystem/*.patch
    fi
    checkfail "vlc source: git clone failed"
fi

cd $WORK_DIR
diagnostic "Setting the environment"
source emsdk/emsdk_env.sh

diagnostic "build libvlc"
cd ./vlc/extras/package/wasm-emscripten/
./build.sh --mode=${SLOW_MODE}

url="http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4"

diagnostic "getting video"
cd $WORK_DIR
mkdir -p samples/
if [ ! -f "./samples/BigBuckBunny.mp4" ]; then
    curl ${url} -o samples/BigBuckBunny.mp4
fi
diagnostic "Generating executable"
./create_main.sh
