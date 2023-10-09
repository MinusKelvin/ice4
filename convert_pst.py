#!/usr/bin/python3

import json, math
with open("0-20.json", "r") as f:
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
sizes = [64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 1, 8, 1, 1, 1, 1, 1, 1, 4, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

data_string = ""
data_string += dump_string(sections[0], "unpack_full(1, PAWN")
data_string += dump_string(sections[eg+0], "unpack_full(0x10000, PAWN")
data_string += dump_string(sections[1], "unpack_full(1, KNIGHT")
data_string += dump_string(sections[eg+1], "unpack_full(0x10000, KNIGHT")
data_string += dump_string(sections[2], "unpack_full(1, BISHOP")
data_string += dump_string(sections[eg+2], "unpack_full(0x10000, BISHOP")
data_string += dump_string(sections[3], "unpack_full(1, ROOK")
data_string += dump_string(sections[eg+3], "unpack_full(0x10000, ROOK")
data_string += dump_string(sections[4], "unpack_full(1, QUEEN")
data_string += dump_string(sections[eg+4], "unpack_full(0x10000, QUEEN")
data_string += dump_string(sections[5], "unpack_full(1, KING")
data_string += dump_string(sections[eg+5], "unpack_full(0x10000, KING")
data_string += dump_string(sections[6], "unpack_full(1, PASSED_PAWN")
data_string += dump_string(sections[eg+6], "unpack_full(0x10000, PASSED_PAWN")

print(f"data string: \"{data_string}\"")

print(f"#define BISHOP_PAIR S({round(sections[11][0])}, {round(sections[eg+11][0])})")
print("int32_t DOUBLED_PAWN[] = {" + ", ".join(
    f"S({-round(v1)}, {-round(v2)})" for v1, v2 in zip(sections[12], sections[eg+12])
) + "};")
print(f"#define TEMPO S({round(sections[13][0])}, {round(sections[eg+13][0])})")
print(f"#define ISOLATED_PAWN S({-round(sections[14][0])}, {-round(sections[eg+14][0])})")
print(f"int32_t PROTECTED_PAWN[] = {{0, S({round(sections[15][0])}, {round(sections[eg+15][0])}), S({round(sections[16][0])}, {round(sections[eg+16][0])})}};")
print(f"#define ROOK_OPEN S({round(sections[17][0])}, {round(sections[eg+17][0])})")
print(f"#define ROOK_SEMIOPEN S({round(sections[18][0])}, {round(sections[eg+18][0])})")
print("int32_t PAWN_SHIELD[] = {" + ", ".join(
    f"S({round(v1)}, {round(v2)})" for v1, v2 in zip(sections[19], sections[eg+19])
) + "};")
print(f"#define KING_OPEN S({round(sections[20][0])}, {round(sections[eg+20][0])})")
print(f"#define KING_SEMIOPEN S({round(sections[21][0])}, {round(sections[eg+21][0])})")
