#!/usr/bin/python3

import json, math
with open("pst-nophase.json", "r") as f:
    data = json.load(f)

def dump_string(piece_data, stuff, extra=None):
    smallest = float("inf")
    largest = -smallest
    average = 0
    for value in piece_data:
        smallest = smallest if smallest < value else value
        largest = largest if largest > value else value
        average += value / len(piece_data)

    scale_factor = max((largest - smallest) / 94, 1.0)

    s = ""
    for value in piece_data:
        c = chr(round((value - smallest) / scale_factor) + 32)
        if c == "\\" or c == "\"":
            s += "\\"
        s += c

    print(f"{stuff}, {scale_factor:.4}, {round(smallest)}", end="")
    if extra:
        for v in extra:
            print(f", {round(smallest + v)}", end="")
    print(f"); // average: {average:.0f}")

    return s

scaled = [v * 160 for v in data["params.weight"][0]]

sections = []
sizes = [64, 64, 64, 64, 64, 64]
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

data_string = ""
data_string += dump_string(sections[0], "unpack_full(PAWN")
data_string += dump_string(sections[5], "unpack_full(KING")
data_string += dump_string(sections[4], "unpack_full(QUEEN")
data_string += dump_string(sections[3], "unpack_full(ROOK")
data_string += dump_string(sections[2], "unpack_full(BISHOP")
data_string += dump_string(sections[1], "unpack_full(KNIGHT")

print(f"data string: \"{data_string}\"")
