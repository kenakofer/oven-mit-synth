#!/bin/bash
 g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` -g src/lv2/lv2OvenMit.cpp -o builds/lv2/linux64/OvenMit.so
