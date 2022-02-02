#!/bin/bash

# For cross compiling Windows 64 bit LV2 on linux using mingw
x86_64-w64-mingw32-g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` src/lv2/lv2OvenMit.cpp -o builds/lv2/win64/OvenMit.so