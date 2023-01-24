#!/usr/bin/python3

import json, math
with open("0-45.json", "r") as f:
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
    print(f"{stuff}, \"", end="")
    for value in piece_data:
        c = chr(round((value - smallest) / scale_factor) + 32)
        if c == "\\" or c == "\"":
            print("\\", end="")
        print(c, end="")
    print(f"\", {scale_factor:.4}, {round(smallest)}", end="")
    if extra:
        for v in extra:
            print(f", {round(smallest + v)}", end="")
    print(f"); // average: {average:.0f}")

scaled = [v * 160 for v in data["params.weight"][0]]

sections = []
sizes = [64, 16, 3, 16, 3, 16, 3, 16, 3, 64, 64, 1, 8, 1, 1, 1, 1, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

for s in [sections[0], sections[10], sections[eg+0], sections[eg+10]]:
    m = float("inf")
    for i in range(8, 56):
        m = min(m, s[i])
    for i in range(8):
        s[i] = m
    for i in range(56, 64):
        s[i] = m

dump_string(sections[0], "unpack_full(0, PAWN")
dump_string(sections[eg+0], "unpack_full(1, PAWN")
dump_string(sections[10], "unpack_full(0, PASSED_PAWN")
dump_string(sections[eg+10], "unpack_full(1, PASSED_PAWN")
dump_string(sections[9], "unpack_full(0, KING")
dump_string(sections[eg+9], "unpack_full(1, KING")
dump_string(sections[7], "unpack_half(0, QUEEN", sections[8])
dump_string(sections[eg+7], "unpack_half(1, QUEEN", sections[eg+8])
dump_string(sections[5], "unpack_half(0, ROOK", sections[6])
dump_string(sections[eg+5], "unpack_half(1, ROOK", sections[eg+6])
dump_string(sections[3], "unpack_half(0, BISHOP", sections[4])
dump_string(sections[eg+3], "unpack_half(1, BISHOP", sections[eg+4])
dump_string(sections[1], "unpack_half(0, KNIGHT", sections[2])
dump_string(sections[eg+1], "unpack_half(1, KNIGHT", sections[eg+2])

print(f"bishop pair: {round(sections[11][0])}, {round(sections[eg+11][0])}")
print(f"doubled pawn mg: {[-round(v) for v in sections[12]]}")
print(f"doubled pawn eg: {[-round(v) for v in sections[eg+12]]}")
print(f"tempo: {round(sections[13][0])}, {round(sections[eg+13][0])}")
print(f"isolated pawn: {-round(sections[14][0])}, {-round(sections[eg+14][0])}")
print(f"single pawn protect: {round(sections[15][0])}, {round(sections[eg+15][0])}")
print(f"double pawn protect: {round(sections[16][0])}, {round(sections[eg+16][0])}")
print(f"rook open file: {round(sections[17][0])}, {round(sections[eg+17][0])}")
print(f"rook semiopen file: {round(sections[18][0])}, {round(sections[eg+18][0])}")
