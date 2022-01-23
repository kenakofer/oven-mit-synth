#!/usr/bin/env python3

from wave import open
from sys import argv
from math import sin, pi


CACHE_PARTIALS = 70
CACHE_SAMPLES = 1024

TYPE = "constexpr std::array<std::array<const float, CACHE_SAMPLES>, CACHE_PARTIALS>"

def get_sin_at(partial_index, sample):
    partial = partial_index + 1
    if partial > 1:
        return 0
    return sin(2 * pi * partial * sample/CACHE_SAMPLES)

def get_saw_at(partial_index, sample):
    partial = partial_index + 1
    factor = 1 / partial
    if partial % 2 == 0:
        factor *= -1

    return factor * sin(2 * pi * partial * sample/CACHE_SAMPLES)

def get_square_at(partial_index, sample):
    partial = partial_index + 1
    if partial % 2 == 0:
        return 0

    return 1 / partial * sin(2 * pi * partial * sample/CACHE_SAMPLES)

def get_triangle_at(partial_index, sample):
    partial = partial_index + 1
    if partial % 2 == 0:
        return 0
    factor = 1 / partial / partial
    if partial % 4 == 3:
        factor *= -1

    return factor * sin(2 * pi * partial * sample/CACHE_SAMPLES)

def build_cache_for_waveform(f):
    cache = []
    cache.append([f(0,s) for s in range(CACHE_SAMPLES)])

    for partial_index in range(1, CACHE_PARTIALS):
        cache.append([cache[partial_index-1][s] + f(partial_index, s) for s in range(CACHE_SAMPLES)])

    return cache

NAME_FUNC_PAIRS = [
    ("SINE", get_sin_at),
    ("TRIANGLE", get_triangle_at),
    ("SQUARE", get_square_at),
    ("SAW", get_saw_at)
]


if __name__ == "__main__":

    print("/* Don't alter this file directly, it is generated from scripts/generateConstCache.py */")
    print()
    print("#ifndef CONSTCACHE_HPP_")
    print("#define CONSTCACHE_HPP_")
    print()
    print("const int CACHE_PARTIALS =", CACHE_PARTIALS, ";")
    print("const int CACHE_SAMPLES =", CACHE_SAMPLES, ";")
    print()

    for (name, func) in NAME_FUNC_PAIRS:
        print(TYPE, "CACHE_"+name, "=")
        print("{{")
        string = ""
        for row in build_cache_for_waveform(func):
            string += "{"
            float_strings = map(lambda v: "{:10.6f}f".format(v), row)
            string += ",".join(float_strings)
            # string += ",".join(str(row))
            string += "},\n"
        string = string[:-2] # Remove final comma and newline
        print(string)
        print("}};")

    print("#endif")