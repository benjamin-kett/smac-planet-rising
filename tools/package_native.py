#!/usr/bin/env python3
"""Package the local native prototype without changing the original installation."""
from pathlib import Path
import plistlib
import shutil
import subprocess

root = Path(__file__).resolve().parent.parent
native = root / 'native'
source = root / 'Alpha Centauri'
assets = native / 'assets'
assets.mkdir(exist_ok=True)
for item in source.iterdir():
    dest = assets / item.name
    if not dest.exists() and not dest.is_symlink():
        dest.symlink_to(item, target_is_directory=item.is_dir())
for name in ('arialnb.ttf', 'arialnbi.ttf', 'arialni.ttf'):
    dest = assets / name
    if not dest.exists() and not dest.is_symlink():
        dest.symlink_to(source / '_backup_v2.1' / name)

runtime = native / 'runtime'
runtime.mkdir(exist_ok=True)
movie = runtime / 'openingx.mp4'
if not movie.exists():
    subprocess.run(['ffmpeg', '-nostdin', '-i', str(source / 'movies/openingx.wve'),
                    '-c:v', 'libx264', '-pix_fmt', 'yuv420p', '-crf', '18',
                    '-c:a', 'aac', '-movflags', '+faststart', str(movie)], check=True)

# Decode the original FLC frames, preserving their timing and magenta transparency.
for name in ('drop_pod', 'alien_pod', 'drop_smoke'):
    atlas = assets / (name + '.png')
    if not atlas.exists():
        subprocess.run(['ffmpeg', '-nostdin', '-v', 'warning', '-i', str(source / (name + '.flc')),
                        '-vf', 'format=rgba,colorkey=0xff00ff:0.01:0,tile=4x8:nb_frames=31',
                        '-frames:v', '1', '-update', '1', str(atlas)], check=True)

contents = native / 'GLSMAC Native.app/Contents'
macos = contents / 'MacOS'
macos.mkdir(parents=True, exist_ok=True)
shutil.copy2(native / 'glsmac/build/bin/GLSMAC', macos / 'GLSMAC')
(contents / 'Info.plist').write_bytes(plistlib.dumps({
    'CFBundleIdentifier': 'local.alphacentauri.glsmac',
    'CFBundleName': 'GLSMAC Native',
    'CFBundleExecutable': 'launch',
    'CFBundlePackageType': 'APPL',
    'NSHighResolutionCapable': False,
}))
launcher = macos / 'launch'
launcher.write_text('''#!/bin/bash
set -euo pipefail
bundle_bin="$(cd "$(dirname "$0")" && pwd)"
root="$(cd "$bundle_bin/../../../.." && pwd)"
cd "$root/native/glsmac"
exec "$bundle_bin/GLSMAC" --smacpath "$root/native/assets" \\
    --prefix "$root/native/runtime" "$@" >> "$root/native/launch.log" 2>&1
''')
launcher.chmod(0o755)
print(contents.parent)
