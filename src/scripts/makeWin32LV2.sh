#!/bin/bash

# For cross compiling Windows 32 bit (i686) LV2 on linux using mingw
i686-w64-mingw32-g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` src/lv2/lv2OvenMit.cpp -o builds/lv2/win32/OvenMit.so