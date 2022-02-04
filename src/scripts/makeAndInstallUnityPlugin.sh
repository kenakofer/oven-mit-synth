#!/bin/bash

# Pass in one argument, which is a path to the Assets directory of a unity
# project. Ovenmit/ directory will be created if it doesn't exist, and the
# plugin files will be copied into it

# Example invocation:
# ./makeAndInstallUnityPlugin.sh ~/MyUnityProject/Assets

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR"/../unity || exit 1
make clean && make nix || exit 1 # Build for linux
make clean && make win || exit 1 # Build for windows
cd ../../ || exit 1
mkdir -p "$1"/OvenMit || exit 1
cp -a builds/unity/* "$1"/OvenMit || exit 1