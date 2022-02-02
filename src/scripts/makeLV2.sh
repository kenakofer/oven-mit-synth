#!/bin/bash
 g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` src/lv2/lv2OvenMit.cpp -o builds/lv2/linux64/OvenMit.so


## Common usage for quick testing, (assuming the output .so is already linked on the system)
#scripts/makeLV2.sh && jalv.gtk3 https://github.com/kenakofer/oven-mit-synth
