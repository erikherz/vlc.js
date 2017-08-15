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
if [ ! -d emsdk_portable ]; then
    diagnostic "Emscripten not found. Fetching it"
    wget https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz
    tar xzf emsdk-portable.tar.gz
    checkfail "Emscripten: fetch failed"
fi

# Compile clang and emscripten
if [ ! -e emsdk_portable/emscripten/master/emcc ]; then
    diagnostic "Emscripten not built. Building it. It will take a LOT of time."

    cd emsdk_portable
    # Fetch the latest registry of available tools.
    ./emsdk update
    # Download and install the latest SDK tools.
    ./emsdk install latest
    # Make the "latest" SDK "active"
    ./emsdk activate latest
    checkfail "Emscripten build failed"
    cd ..
fi

diagnostic "Setting the environment"
# Export the correct path to get everything working
#export PATH=$PWD/emsdk_portable:$PWD/emsdk_portable/clang/fastcomp/build_master_64/bin:$PWD/emsdk_portable/node/4*64bit/bin:$PWD/emsdk_portable/emscripten/master:$PATH
cd emsdk_portable && . ./emsdk_set_env.sh && cd ..

# Check that clang is working
clang --version

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
cd ../..

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

CC=emcc CXX=em++ AR=emar RANLIB=emranlib \
    ../bootstrap --disable-disc --disable-gpl --disable-sout \
    --disable-network \
    --host=x86_64-linux-gnu
checkfail "contribs: bootstrap failed"

make list
#make $MAKEFLAGS fetch
#checkfail "contribs: make fetch failed"

CC=emcc CXX=em++ AR=emar RANLIB=emranlib make $MAKEFLAGS \
    .zlib
checkfail "contribs: make failed"

cd ../../

# Build
mkdir -p build-emscripten && cd build-emscripten
CC=emcc CXX=em++ AR=emar RANLIB=emranlib \
    ../configure --host=x86_64-linux-gnu --build=asmjs-unknown-emscripten

