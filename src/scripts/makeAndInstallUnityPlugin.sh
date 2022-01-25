#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "$SCRIPT_DIR"/../unity || exit 1
make clean && make || exit 1
cd ../../ || exit 1
mkdir -p "$1"/OvenMit || exit 1
cp -a builds/unity/* "$1"/OvenMit || exit 1