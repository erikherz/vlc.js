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


# Download the portable SDK and uncompress it
if [ ! -d emsdk-portable ]; then
    diagnostic "Emscripten not found. Fetching it"
    wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
    tar xzf emsdk-portable.tar.gz
    checkfail "Emscripten: fetch failed"
fi

# Compile clang and emscripten
if [ ! -e emsdk-portable/emscripten/incoming/emcc ]; then
    diagnostic "Emscripten not built. Building it. It will take a LOT of time."

    cd emsdk-portable
    # Fetch the latest registry of available tools.
    ./emsdk update
    # Fetch the latest tags
    ./emsdk update-tags
    # Download and install the latest SDK tools.
    ./emsdk install --build=Release emscripten-incoming-64bit
    # Make the "latest" SDK active
    ./emsdk activate --build=Release emscripten-incoming-64bit
    checkfail "Emscripten build failed"
    # Download and install the corresponding node version
    ./emsdk install --build=Release node-4.1.1-64bit
    ./emsdk activate --build=Release node-4.1.1-64bit
    checkfail "Node build failed"
    # Download and install the corresponding clang version
    ./emsdk install --build=Release clang-incoming-64bit
	./emsdk activate --build=Release clang-incoming-64bit
    checkfail "Clang build failed"
    cd ..
fi

# Go go go vlc
if [ ! -d "vlc" ]; then
    diagnostic "VLC source not found, cloning"
    git clone http://git.videolan.org/git/vlc.git vlc
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
cd ../../..

diagnostic "Setting the environment"
# Export the correct path to get everything working
#export PATH=$PWD/emsdk_portable:$PWD/emsdk_portable/clang/fastcomp/build_incoming_64/bin:$PWD/emsdk_portable/node/4*64bit/bin:$PWD/emsdk_portable/emscripten/incoming:$PATH
cd emsdk-portable && . ./emsdk_env.sh && cd ..
export PKG_CONFIG_PATH=$EMSDK/emscripten/incoming/system/lib/pkgconfig
export PKG_CONFIG_LIBDIR=$PWD/vlc/contrib/asmjs_unknowm_emscripten/lib/pkgconfig
export PKG_CONFIG_PATH_CUSTOM=$PKG_CONFIG_LIBDIR
export CC=emcc
export LD=emcc
export LDSHARED=emcc
export NM=llvm_nm
export CXX=em++
export AR=emar
export RANLIB=emranlib

# Check that clang is working
clang --version

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
    --host=asmjs-unknown-emscripten --build=x86_64-linux
checkfail "contribs: bootstrap failed"

make list
make $MAKEFLAGS fetch
checkfail "contribs: make fetch failed"

    make $MAKEFLAGS \
    .zlib .libmpeg2 .png .dshow .goom .modplug .lua .luac \
    .jpeg .openjpeg .zvbi .opus .xau .dca .glew .lame \
	.twolame .fribidi .aribb24 .ogg .vorbis .crystalhd \
	.theora .tremor .bpg .nfs
	#.mad .xproto

    CFLAGS="-D_FILE_OFFSET_BITS=64" \
    make $MAKEFLAGS .mpg123

checkfail "contribs: make failed"

cd ../../

# Build
mkdir -p build-emscripten && cd build-emscripten

OPTIONS="
    --host=asmjs-unknown-emscripten
    --enable-debug
    --enable-gles2
    --disable-nls
    --disable-sout
    --disable-vlm
    --disable-addonmanagermodules
    --disable-avcodec
    --enable-merge-ffmpeg
    --disable-swscale
    --disable-a52
    --disable-x264
    --disable-xcb
    --disable-xvideo
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

# Note :
#        search.h is a blacklisted module
#        time.h is a blacklisted module
    ac_cv_func_sendmsg=yes ac_cv_func_recvmsg=yes ac_cv_func_if_nameindex=yes ac_cv_header_search_h=no ac_cv_header_time_h=no \
../configure ${OPTIONS}

make ${MAKEFLAGS}
