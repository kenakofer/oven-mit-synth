#!/bin/bash

# Makes builds for linux, win32 and win64, placing them in the builds/unity/ directory.

# Example invocation:
# ./makeUnityPlugin.sh

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR"/../unity || exit 1
make clean && make nix || exit 1 # Build for linux
make clean && make win32 || exit 1 # Build for windows
make clean && make win64 || exit 1 # Build for windows