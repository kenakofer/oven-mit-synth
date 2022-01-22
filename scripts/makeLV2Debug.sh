#!/bin/bash
 g++ -fvisibility=hidden -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` -g harmonicSynth.cpp -o harmonicSynth.so
