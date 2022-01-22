#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ~/.lv2 || exit 1
mkdir -p OvenMit
cd OvenMit
ln -s "$SCRIPT_DIR"/../bin/lv2/*.so ./
ln -s "$SCRIPT_DIR"/../src/lv2/*.ttl ./