# VLC.js: VLC for WASM/asm.js

This project is a port of VLC to run completely in the web-browser.

It is, of course, quite insane, but it will be doable in the future, as WASM improves.

It started with an asm.js port, now that WASM is ready with :
 - threads
 - WASM (and not only asm.js)
 - SIMD.js/wasm
 - Full webgl support
 - Audio support in threads

we are getting closer to a first version.

After, when we have modules, then VLC modules could be downloaded at runtime.

## Installation

### Update the config.sub in your system.

```bash
cp config.sub /usr/share/automake-1.15/config.sub
```

### Compile vlc.

```bash
./compile.sh
```

You can run it locally by launching a webserver on your computer.

```bash
source emsdk/emsdk_env.sh
emrun --no_browser --port 8080 vlc.html --emrun
```

And seeing the result at url : http://localhost:8080/vlc.html
You can also put the generated files on a server and access it normally.

## Run

It should be able to run in all browsers and OSes except Chrome in MacOS where there is a [bug](https://code.videolan.org/b1ue/vlc/issues/36) 

|| Chrome 77.0.3865.120 (stable) | Firefox Dev Edition (70.0b14) (*)|
| ------ | ------ | ------ |
| Linux (Arch) | Audio+Video | Audio+Video |
| MacOS | #36 | Audio+Video |
| Windows 10 | Audio+Video | Audio+Video (#35)|


(*) Type about:config in the Location Bar and press Enter to see all Preferences.

Features to enable :
 * Set 'javascript.options.shared_memory' to true
 * Set 'gfx.webrender.all' to true
 * Set 'dom.postMessage.sharedArrayBuffer.bypassCOOP_COEP.insecure.enabled' to true

Then, you should be able to see vlc.js.

## Hack

To compile vlc.js, we need to apply patches on VLC master.

All the patches are grouped in this folder:
 * patch_vlc: vlc patches

There are also patches to enable dav1d and dvd contribs.

The script compile.sh sets up the toolchain, clones vlc, compiles it, generates the module_list and compiles all into a html + js pages.

To generate the module_list it calls a subscript which will generate a C file with all modules and entry points, because that is required for a static build of VLC.

