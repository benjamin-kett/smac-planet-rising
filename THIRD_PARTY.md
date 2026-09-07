# Source provenance

`native/glsmac` is derived from https://github.com/afwbkbc/glsmac at commit
`4c4412225158d078ba1b3bc136db3826f54c165c`. Its license is preserved at
`native/glsmac/LICENSE` (GNU Affero General Public License version 3), together
with upstream attribution and the bundled Terminus font license.

Local modifications include the native macOS main-thread integration, OpenGL
core-profile support, AVKit intro playback, expansion startup and landing
animations, and worker shutdown ordering. See `native/README.md` and Git history.

The original commercial game installation, linked game assets, decoded movies
and animation atlases are not part of this source repository. Building the
playable application requires a separately supplied installation.

Thinker and OpenSMACX are research references recorded in `docs/ENGINEERING.md`.
Their local checkouts are excluded from this repository; no claim is made that
either is a complete portable game engine.
