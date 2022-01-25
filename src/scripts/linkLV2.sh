#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ~/.lv2 || exit 1
mkdir -p OvenMit
cd OvenMit
ln -sf "$SCRIPT_DIR"/../../builds/lv2/*.so ./
ln -sf "$SCRIPT_DIR"/../lv2/*.ttl ./