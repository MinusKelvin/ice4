#!/usr/bin/python3

EXTRA_SCALE = 1.5

import json, math
with open("0-45.json", "r") as f:
    data = json.load(f)

small = [float("inf")]*4
large = [-s for s in small]
for vector in data["ft.weight"]:
    for i, value in enumerate(vector):
        small[i] = min(small[i], value)
        large[i] = max(large[i], value)

scale = 94 / max(l - s for l, s in zip(large, small))
print("FT offsets:", [round(s * scale) for s in small])
print("FT scale (internal):", scale)

def dump_ft(piece_data, stuff):
    print(f"unpack_ft({stuff}, \"", end="")
    for vector in piece_data:
        for i, value in enumerate(vector):
            c = chr(round((value - small[i]) * scale) + 32)
            if c == "\\" or c == "\"":
                print("\\", end="")
            print(c, end="")
    print("\");")

dump_ft(data["ft.weight"][0:32], "PAWN")
dump_ft(data["ft.weight"][32:64], "KNIGHT")
dump_ft(data["ft.weight"][64:96], "BISHOP")
dump_ft(data["ft.weight"][96:128], "ROOK")
dump_ft(data["ft.weight"][128:160], "QUEEN")
dump_ft(data["ft.weight"][160:192], "KING")
print("FT bias:", [round((value - small[i]) * scale) for i, value in enumerate(data["ft.bias"])])

print()

out_weight = [v[0] for v in data["out.weight"]]
small = float("inf")
large = -small
for v in out_weight:
    small = min(small, v)
    large = max(large, v)
out_scale = 94 / (large - small)
print(f"Out offset: {round(small * out_scale)}")
print(f"Out scale: {EXTRA_SCALE / out_scale:.4}")

print("\"", end="")
for value in out_weight:
    c = chr(round((value - small) * out_scale) + 32)
    if c == "\\" or c == "\"":
        print("\\", end="")
    print(c, end="")
print("\"")
print(f"Out bias: {data['out.bias'][0] * scale * EXTRA_SCALE:.4}")

print()

def dump_pst(piece_data, stuff):
    smallest = float("inf")
    largest = -smallest
    average = 0
    for value in piece_data:
        smallest = smallest if smallest < value else value
        largest = largest if largest > value else value
        average += value / 32

    scale_factor = 94 / (largest - smallest)
    print(f"unpack_pst({stuff}, \"", end="")
    for value in piece_data:
        c = chr(round((value - smallest) * scale_factor) + 32)
        if c == "\\" or c == "\"":
            print("\\", end="")
        print(c, end="")
    print(f"\", {EXTRA_SCALE*scale/scale_factor:.4}, {round(EXTRA_SCALE*smallest * scale_factor)}); // average: {average * scale_factor:.0f}")

scaled = [v[0] for v in data["pst"]]

dump_pst(scaled[0:32], "PAWN")
dump_pst(scaled[32:64], "KNIGHT")
dump_pst(scaled[64:96], "BISHOP")
dump_pst(scaled[96:128], "ROOK")
dump_pst(scaled[128:160], "QUEEN")
dump_pst(scaled[160:192], "KING")
