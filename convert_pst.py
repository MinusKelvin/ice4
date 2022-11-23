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
        average += value / 32

    scale_factor = max((largest - smallest) / 94, 1.0)
    print(f"unpack({stuff}, \"", end="")
    for value in piece_data:
        c = chr(round((value - smallest) / scale_factor) + 32)
        if c == "\\" or c == "\"":
            print("\\", end="")
        print(c, end="")
    print(f"\", {scale_factor:.4}, {round(smallest)}); // average: {average:.0f}")

scaled = [v[0] * 160 for v in data["pst.weight"]]

dump_string(scaled[0:32], "0, PAWN")
dump_string(scaled[32:64], "0, KNIGHT")
dump_string(scaled[64:96], "0, BISHOP")
dump_string(scaled[96:128], "0, ROOK")
dump_string(scaled[128:160], "0, QUEEN")
dump_string(scaled[160:192], "0, KING")
dump_string(scaled[192:224], "1, PAWN")
dump_string(scaled[224:256], "1, KNIGHT")
dump_string(scaled[256:288], "1, BISHOP")
dump_string(scaled[288:320], "1, ROOK")
dump_string(scaled[320:352], "1, QUEEN")
dump_string(scaled[352:384], "1, KING")
