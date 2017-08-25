# VLC.js: VLC for WASM/asm.js

This project is a port of VLC to run completely in the webbrowser.

It is, of course, quite insane, but it will be doable in the future, as WASM improves.

Notably when we have:
 - threads
 - WASM (and not only asm.js)
 - SIMD.js/wasm
 - Full webgl support

we will be able to have a first version.

After, when we have modules, then VLC modules could be downloaded at runtime.

## Installation

Update the config.sub in your system.

```bash
cp config.sub /usr/share/automake-1.15/config.sub
```

Compile vlc.

```bash
./compile.sh
```
