#!/usr/bin/env python3

from wave import open
from sys import argv
from array import array

CACHE_NOISE_PARTIALS = 30
# PARTIAL_MULTIPLIERS = range(2,CACHE_NOISE_PARTIALS)
PARTIAL_MULTIPLIERS = [(p+1, 1.85 + .95*p) for p in range(0,CACHE_NOISE_PARTIALS-2)]

def build_cache_for_waveform(samples):
    cache = []
    cache.append(samples)
    length = len(samples)

    for i, multiplier in PARTIAL_MULTIPLIERS:
        cache.append(
            [cache[i-1][s] + samples[int(s*multiplier)%length] for s in range(length)]
        )
    return cache


if __name__ == "__main__":
    if len(argv) < 2:
        print("Pass in a wav file!")
        exit(1)
    with open(argv[1], 'r') as w:
        variable_name = argv[1].split("/")[-1].split(".")[0].upper()

        frames = list(map(lambda i: i / (2**15), array('h', w.readframes(-1))))

        print("#ifndef", variable_name+"_HPP")
        print("#define", variable_name+"_HPP")
        print()
        print("#include <array>")
        print()
        print("constexpr int", variable_name+"_LENGTH =", len(frames), ";")
        print("constexpr int", variable_name+"_PARTIALS =", CACHE_NOISE_PARTIALS, ";")
        print()


        typestring = "constexpr std::array<std::array<const float, "+str(len(frames))+">, "+str(CACHE_NOISE_PARTIALS)+">"
        print(typestring, "CACHE_"+variable_name, "=")
        print("{{")
        string = ""
        for row in build_cache_for_waveform(frames):
            string += "{"
            float_strings = map(lambda v: "{:10.6f}f".format(v), row)
            string += ",".join(float_strings)
            string += "},\n"
        string = string[:-2] # Remove final comma and newline
        print(string)
        print("}};")
        print("#endif")
