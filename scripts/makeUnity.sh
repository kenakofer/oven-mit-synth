#!/bin/bash
 g++ -fvisibility=hidden -I. -O3 -ffast-math -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -rdynamic -pthread `pkg-config --cflags lv2` -lm `pkg-config --libs lv2` AudioPluginUtil.cpp Plugin_OvenMit.cpp -o PluginOvenMit.so
