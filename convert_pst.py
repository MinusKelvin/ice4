#!/usr/bin/python3

import json, math
with open("0-45.json", "r") as f:
    data = json.load(f)

def dump_string(piece_data, stuff):
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
    print(f"\", {scale_factor:.4}, {round(smallest)}); // average: {average:.0f}")

scaled = [v * 160 for v in data["params.weight"][0]]

sections = []
sizes = [64, 32, 32, 32, 32, 64, 64, 1, 8, 1, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

for s in [sections[0], sections[6], sections[eg+0], sections[eg+6]]:
    m = float("inf")
    for i in range(8, 56):
        m = min(m, s[i])
    for i in range(8):
        s[i] = m
    for i in range(56, 64):
        s[i] = m

dump_string(sections[0], "unpack_full(0, PAWN")
dump_string(sections[eg+0], "unpack_full(1, PAWN")
dump_string(sections[6], "unpack_full(0, PASSED_PAWN")
dump_string(sections[eg+6], "unpack_full(1, PASSED_PAWN")
dump_string(sections[5], "unpack_full(0, KING")
dump_string(sections[eg+5], "unpack_full(1, KING")
dump_string(sections[4], "unpack_half(0, QUEEN")
dump_string(sections[eg+4], "unpack_half(1, QUEEN")
dump_string(sections[3], "unpack_half(0, ROOK")
dump_string(sections[eg+3], "unpack_half(1, ROOK")
dump_string(sections[2], "unpack_half(0, BISHOP")
dump_string(sections[eg+2], "unpack_half(1, BISHOP")
dump_string(sections[1], "unpack_half(0, KNIGHT")
dump_string(sections[eg+1], "unpack_half(1, KNIGHT")

print(f"bishop pair: {round(sections[7][0])}, {round(sections[eg+7][0])}")
print(f"doubled pawn mg: {[-round(v) for v in sections[8]]}")
print(f"doubled pawn eg: {[-round(v) for v in sections[eg+8]]}")
print(f"tempo: {round(sections[9][0])}, {round(sections[eg+9][0])}")
print(f"isolated pawn: {-round(sections[10][0])}, {-round(sections[eg+10][0])}")
print(f"pawn chaining: {round(sections[11][0])}, {round(sections[eg+11][0])}")
