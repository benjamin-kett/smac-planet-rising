# Native launch verification — 2026-09-07

Host: Apple M1 Pro, ARM64, macOS 15.7.3.

The binary at `GLSMAC Native.app/Contents/MacOS/GLSMAC` was identified by `file`
as a Mach-O 64-bit ARM64 executable. Runtime logging reported Apple OpenGL 4.1
and the Apple M1 Pro renderer. No Windows executable was launched.

## Observed through native app control

1. The GLSMAC main menu rendered using the original Alpha Centauri artwork.
2. Mouse input navigated Start Game → Make Random Map → Small Planet → Talent
   → Play with Standard Rules. The map rendered with terrain, a base, a unit,
   and the turn interface at Mission Year 2101.
3. After adding the native movie player, a fresh process played the original
   opening movie. Screenshots showed distinct frames at approximately 00:25
   and 01:56; the player showed a 02:39 duration. Playback was allowed to finish
   without seeking or skipping. The log recorded `Native intro playback completed`.
4. The movie window closed automatically and the engine showed its logo followed
   by the main menu. Keyboard input navigated the same new-game setup again.
   A final screenshot confirmed the new map, University Base, a selected unit,
   and Mission Year 2101. The app was left running at that map.

These checks verify the GLSMAC prototype startup flow, not the original game's
complete rules, campaign, faction selection, AI, or save compatibility.

## Other checks

* ARM64 compilation and linking succeeded.
* `git diff --check` passed for the modified tracked engine sources.
* Isolated GSE empty-script test: passed, exit 0 (`gse-smoke.log`).
* Full GSE script suite: crashed, exit 139 (`gse-tests.log`).
* Earlier in-game quit: GC assertion; subsequently fixed as described below.
* Movie audio stream is non-silent (mean -19.8 dB, peak -0.5 dB), but speaker
  output was not independently verified. Conversion logged a malformed source
  audio packet. The resulting MP4 decodes and lasts 158.73 seconds.

Logs are local to `native/`. This is a development build with known failures.

## Alien Crossfire follow-up

* Rebuilt the ARM64 binary and packaged it with the expansion opening movie and
  menu artwork. Watched the approximately 73-second opening movie finish; the
  native player logged completion and returned to the menu.
* Navigated Start Game → Make Random Map → Tiny Planet → Citizen → Standard
  Rules → Alien Crossfire Factions → Cybernetic Consciousness. Observed the
  human pod animation over Alpha Prime, then moved the selected unit from
  `(35,15)` to `(33,15)` after the animation completed.
* Started Caretakers via the quickstart faction flag. Observed Decision: Manifold
  on the map; logs recorded the alien pod and smoke sequence completing. Unit
  movement succeeded afterward. The short alien animation was not captured
  visually during this run, so its frame fidelity is not independently verified.
* Focused GSE startup tests exercise all fourteen faction callbacks, planetfall
  ownership/turn validation, and the human/alien animation selection. The test
  runner's text result must be checked because it can exit zero on failed tests.
* Applied the chosen difficulty when constructing the player instead of always
  using the default. Full difficulty-rule behavior is not validated.
* A macOS crash report traced the quit assertion to `Game::Iterate()` submitting
  work into the freed GC space. Workers now stop and join after frontend teardown
  and before script-engine destruction. Relaunched Caretakers, used the in-game
  quit dialog, and observed exit status **0**, with all worker threads and the
  OpenGL window stopped. GC object cleanup warnings remain.

This verifies expansion startup assets and prototype selection/animation paths.
It does not verify a playable Alien Crossfire campaign. See `PORT_COVERAGE.md`
in `docs/` for the explicit remaining requirements.
