# Native macOS prototype

This is a locally ported GLSMAC engine, based on upstream commit
`4c4412225158d078ba1b3bc136db3826f54c165c`. It is an ARM64 Mach-O program,
using native macOS OpenGL/SDL and AVFoundation. It does not run the Windows
executable and does not use Wine or CPU emulation.

**This is not a complete port of the original Alpha Centauri game.** GLSMAC is
an unfinished reimplementation. Its map, menus and initial units run, but the
original campaign rules, AI, diplomacy, faction setup and save compatibility
are not complete. Its default world setup differs from the original game.

## Launch

Open `GLSMAC Native.app` in this directory. Keep it in this workspace: the
launcher locates the engine, assets and runtime relative to the app bundle.
Escape skips the opening movie. After the movie, use Start Game and select a
random map, planet size, difficulty, standard rules and a faction to enter a map.
The default movie and menu artwork are Alien Crossfire. The faction chooser offers
the seven expansion factions and the seven original factions.

Runtime files live in `native/runtime`; logs go to `native/launch.log`.
`native/assets` links to the original installation, with missing narrow fonts
resolved from the existing `_backup_v2.1` directory. The original files are not
modified. `runtime/openingx.mp4` is a local transcode of `movies/openingx.wve`.
The pod and smoke atlases are decoded from the original FLC animation files.
Do not distribute the linked assets or movie with the source code.

## Rebuild

Dependencies installed with Homebrew:

```sh
brew install cmake pkgconf sdl2 sdl2_image ossp-uuid glew yaml-cpp freetype ffmpeg
bash tools/build_native.sh
```

Run the build command from the workspace root. The packaging script preserves
an existing converted movie. The app uses local Homebrew libraries; it is not
a standalone distributable bundle. macOS 15.7.3 / Apple M1 Pro is the tested host.

## Changes from upstream

* Execute window creation, input, rendering and the main scheduler on the
  process main thread, as Cocoa requires.
* Request a forward-compatible OpenGL 4.1 core context and bind a vertex array
  object; enable GLEW core-profile loading and avoid a removed legacy GL hint.
* Include the standard mutex header explicitly and repair FastDebug references
  to DEBUG-only memory instrumentation.
* Play the opening movie using an AVKit window inside the engine process;
  return to the engine's startup/menu flow at completion or Escape.
* Add repeatable asset staging and macOS app packaging scripts.
* Add expansion-first faction selection and apply the selected difficulty.
* Play the original human or alien pod frames at the starting unit tile, followed
  by smoke and the landing sound. Release the animation tile lock on completion.
* Support silent animations without trying to load an empty sound filename.

## Known limitations observed during testing

* The complete GSE script suite terminated with a segmentation fault after its
  benchmark script; it did not pass. The isolated `empty.gls.js` smoke test
  passed and exited with status 0. LLDB could not attach on this host, so the
  full-suite crash remains undiagnosed.
* The in-game quit assertion was fixed by joining worker threads before freeing
  the shared script state. A new in-game quit exited 0; GC cleanup warnings remain.
* Speaker output has not been independently recorded or verified. The expansion
  movie was observed playing to completion (approximately 73 seconds).
* This is a FastDebug build, with verbose logs and upstream diagnostic behavior.
* No full campaign or original save-file compatibility has been validated.

See `TESTING.md` for observed results and `../docs/PORT_COVERAGE.md` for the
remaining engine requirements. Expansion assets do not establish expansion rules parity.
