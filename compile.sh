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
WORK_DIR=$PWD

EMSDK_VERSION="2.0.17"
# Download the portable SDK and uncompress it
if [ ! -d emsdk ]; then
    diagnostic "emsdk not found. Fetching it"
    git clone http://github.com/emscripten-core/emsdk.git emsdk
    cd emsdk && ./emsdk update-tags && ./emsdk install ${EMSDK_VERSION} && ./emsdk activate ${EMSDK_VERSION}
    checkfail "emsdk: fetch failed"
fi

cd $WORK_DIR
TESTED_HASH="ae406f9e6be4eb0b7d6b8db6edd51005d5af546f"
# Go go go vlc
if [ ! -d vlc ]; then
    diagnostic "VLC source not found, cloning"
    git clone https://code.videolan.org/videolan/vlc.git vlc || checkfail "VLC source: git clone failed"
    cd vlc
    diagnostic "VLC source: resetting to the TESTED_HASH commit (${TESTED_HASH})"
    git reset --hard ${TESTED_HASH} || checkfail "VLC source: TESTED_HASH ${TESTED_HASH} not found"
    cd ..
    checkfail "vlc source: git clone failed"
fi

cd vlc

# Make in //
if [ -z "$MAKEFLAGS" ]; then
    UNAMES=$(uname -s)
    MAKEFLAGS=
    if which nproc >/dev/null; then
        MAKEFLAGS=-j`nproc`
    elif [ "$UNAMES" == "Darwin" ] && which sysctl >/dev/null; then
        MAKEFLAGS=-j`sysctl -n machdep.cpu.thread_count`
    fi
fi

# VLC tools
export PATH=`pwd`/extras/tools/build/bin:$PATH
echo "Building tools"
cd extras/tools
./bootstrap
checkfail "buildsystem tools: bootstrap failed"
make $MAKEFLAGS
checkfail "buildsystem tools: make"

cd $WORK_DIR

diagnostic "Setting the environment"
source emsdk/emsdk_env.sh
export PKG_CONFIG_PATH=$EMSDK/emscripten/incoming/system/lib/pkgconfig
export PKG_CONFIG_LIBDIR=$PWD/vlc/contrib/wasm32_unknowm_emscripten/lib/pkgconfig
export PKG_CONFIG_PATH_CUSTOM=$PKG_CONFIG_LIBDIR

# Check that clang is working
clang --version

diagnostic "Patching"

cd vlc

# patching vlc
if [ -d ../vlc_patches ] && [ "$(ls -A ../vlc_patches)" ]; then
    # core patches
    git am -3 ../vlc_patches/upstream-core/*.patch
fi

# BOOTSTRAP

if [ ! -f configure ]; then
    echo "Bootstraping"
    ./bootstrap
    checkfail "vlc: bootstrap failed"
fi

############
# Contribs #
############

echo "Building the contribs"
mkdir -p contrib/contrib-emscripten
cd contrib/contrib-emscripten

    ../bootstrap --disable-disc --disable-gpl --disable-sout \
    --disable-network \
    --host=wasm32-unknown-emscripten --build=x86_64-linux
checkfail "contribs: bootstrap failed"

emmake make list
emmake make $MAKEFLAGS fetch
checkfail "contribs: make fetch failed"
emmake make $MAKEFLAGS .ffmpeg

checkfail "contribs: make failed"

cd ../../

# Build
mkdir -p build-emscripten && cd build-emscripten

OPTIONS="
    --host=wasm32-unknown-emscripten
    --enable-debug
    --enable-gles2
    --disable-lua
    --disable-ssp
    --disable-nls
    --disable-sout
    --disable-vlm
    --disable-addonmanagermodules
    --enable-avcodec
    --enable-merge-ffmpeg
    --disable-swscale
    --disable-a52
    --disable-x264
    --disable-xcb
    --disable-alsa
    --disable-macosx
    --disable-sparkle
    --disable-qt
    --disable-screen
    --disable-xcb
    --disable-pulse
    --disable-alsa
    --disable-oss
    --disable-vlc"
#     --disable-xvideo Unknown option
# Note :
#        search.h is a blacklisted module
#        time.h is a blacklisted module
#        shm.h is a blacklisted module
#        ssp is not supported on the wasm backend

emconfigure ../configure ${OPTIONS}  \
	    ac_cv_func_sendmsg=yes ac_cv_func_recvmsg=yes ac_cv_func_if_nameindex=yes ac_cv_header_search_h=no ac_cv_header_time_h=no ac_cv_header_sys_shm_h=no

emmake make ${MAKEFLAGS}

diagnostic "Generating module list"
cd ../..
./generate_modules_list.sh
cd vlc/build-emscripten
emcc vlc-modules.c -o vlc-modules.bc -pthread
cd ../..

url="http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4"

# copy Dolby_Canyon.vob
diagnostic "getting video"
cd vlc/build-emscripten/
curl ${url} -o BigBuckBunny.mp4

cd $WORK_DIR

diagnostic "Generating executable"
cp main.c vlc/build-emscripten/
./create_main.sh
