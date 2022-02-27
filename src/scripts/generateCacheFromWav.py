#!/usr/bin/env python3

from wave import open
from sys import argv
from array import array




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
        print()


        typestring = "constexpr std::array<const float, "+str(len(frames))+">"
        print(typestring, "CACHE_"+variable_name, "=")
        print("{{")
        float_strings = map(lambda v: "{:10.6f}f".format(v), frames)
        string = ",".join(float_strings)
        print(string)
        print("}};")
        print("#endif")
