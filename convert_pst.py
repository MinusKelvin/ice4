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
sizes = [48, 16, 3, 16, 3, 16, 3, 16, 3, 16, 48, 1, 8, 1, 1, 1, 1, 1, 1, 4, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

data_string = ""
data_string += dump_string(sections[0], "unpack_full(0, PAWN")
data_string += dump_string(sections[eg+0], "unpack_full(16, PAWN")
data_string += dump_string(sections[10], "unpack_full(0, PASSED_PAWN")
data_string += dump_string(sections[eg+10], "unpack_full(16, PASSED_PAWN")
data_string += dump_string(sections[9], "unpack_smol(0, KING")
data_string += dump_string(sections[eg+9], "unpack_smol(16, KING")
data_string += dump_string(sections[7], "unpack_half(0, QUEEN", sections[8])
data_string += dump_string(sections[eg+7], "unpack_half(16, QUEEN", sections[eg+8])
data_string += dump_string(sections[5], "unpack_half(0, ROOK", sections[6])
data_string += dump_string(sections[eg+5], "unpack_half(16, ROOK", sections[eg+6])
data_string += dump_string(sections[3], "unpack_half(0, BISHOP", sections[4])
data_string += dump_string(sections[eg+3], "unpack_half(16, BISHOP", sections[eg+4])
data_string += dump_string(sections[1], "unpack_half(0, KNIGHT", sections[2])
data_string += dump_string(sections[eg+1], "unpack_half(16, KNIGHT", sections[eg+2])

print(f"data string: \"{data_string}\"")

print(f"#define BISHOP_PAIR S({round(sections[11][0])}, {round(sections[eg+11][0])})")
print("uint32_t DOUBLED_PAWN[] = {" + ", ".join(
    f"S({-round(v1)}, {-round(v2)})" for v1, v2 in zip(sections[12], sections[eg+12])
) + "};")
print(f"#define TEMPO S({round(sections[13][0])}, {round(sections[eg+13][0])})")
print(f"#define ISOLATED_PAWN S({-round(sections[14][0])}, {-round(sections[eg+14][0])})")
print(f"uint32_t PROTECTED_PAWN[] = {{0, S({round(sections[15][0])}, {round(sections[eg+15][0])}), S({round(sections[16][0])}, {round(sections[eg+16][0])})}};")
print(f"#define ROOK_OPEN S({round(sections[17][0])}, {round(sections[eg+17][0])})")
print(f"#define ROOK_SEMIOPEN S({round(sections[18][0])}, {round(sections[eg+18][0])})")
print("uint32_t PAWN_SHIELD[] = {" + ", ".join(
    f"S({round(v1)}, {round(v2)})" for v1, v2 in zip(sections[19], sections[eg+19])
) + "};")
print(f"#define KING_OPEN S({round(sections[20][0])}, {round(sections[eg+20][0])})")
print(f"#define KING_SEMIOPEN S({round(sections[21][0])}, {round(sections[eg+21][0])})")
