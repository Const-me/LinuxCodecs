#!/bin/bash
set -euo pipefail
script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
build_dir="$script_dir/x64/cmake"

clear
cmake -S "$script_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release
cmake --build "$build_dir" --parallel

# Comment the following line to keep debug symbols in the DLL
strip AudioCodecs/x64/linux/bin/AudioCodecs.amd64.so