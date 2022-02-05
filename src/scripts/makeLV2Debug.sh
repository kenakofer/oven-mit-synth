#!/bin/bash
 g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` -g src/lv2/lv2OvenMit.cpp -o builds/lv2/linux64/OvenMit.so


## To test with jalv, for example:
# src/scripts/makeLV2Debug.sh && gdb jalv.gtk3
## Then type
# run https://github.com/kenakofer/oven-mit-synth

# If you do this, note that using Ctrl-c to interrupt will interrupt the gtk
# thread. If you want to interrupt the oven mit logic thread, you'll need to
# find its PID, then `kill -INT` it.
