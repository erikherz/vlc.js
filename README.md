# VLC.js: VLC for WASM/asm.js

This project is a port of VLC to run completely in the webbrowser.

It is, of course, quite insane, but it will be doable in the future, as WASM improves.

Notably when we have:
 - threads
 - WASM (and not only asm.js)
 - SIMD.js/wasm
 - Full webgl support
 - Audio support in threads

we will be able to have a first version.

After, when we have modules, then VLC modules could be downloaded at runtime.

## Installation

Update the config.sub in your system.

```bash
cp config.sub /usr/share/automake-1.15/config.sub
```

Before compiling vlc, you must copy the video Dolby_Canyon.vob in the root of this project.

Compile vlc.

```bash
./compile.sh
```

All files (javascript and html) are in the folder build-emscripten. You can see localy vlc by
lanch a webserver on your computer.

```bash
source emsdk-portable/emsdk_env.sh
cd vlc/build-emscripten
emrun --no_browser --port 8080 experimental.html --emrun
```

And seeing the result in firefox at url : http://localhost:8080/experimental.html
You can also put the generated files on a server and access to it normaly.

## Watching

Currently only firefox is able to run vlc.js, but you have to enable some features in firefox.
Type about:config in the Location Bar and press Enter to see all Preferences.

Features to enable :
 * Set 'javascript.options.shared_memory' to true
 * Set 'gfx.offscreencanvas.enabled' to true

Then you can see vlc.js

## Structure

To can compile vlc, we apply patch to it. Emscripten being in devlopment, some patch have to be applied to fix problems.
All patchs are grouped in folders :
 * patch_vlc : all patch for vlc
 * patch_emscripten : all patchs for emscripten

The script compile.sh set up the toolchain, clone vlc, compile it, generate the module_list and compile all into an html + js pages.
To generate the module_list it call a subscript which will generate a C file with all modules and enty points.
