#!/bin/bash
set -euo pipefail
root="$(cd "$(dirname "$0")/.." && pwd)"
cmake -S "$root/native/glsmac" -B "$root/native/glsmac/build" \
    -DCMAKE_BUILD_TYPE=FastDebug -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build "$root/native/glsmac/build" -j 8
python3 "$root/tools/package_native.py"
