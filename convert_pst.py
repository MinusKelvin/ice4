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
    print(f"unpack({stuff}, \"", end="")
    for value in piece_data:
        c = chr(round((value - smallest) / scale_factor) + 32)
        if c == "\\" or c == "\"":
            print("\\", end="")
        print(c, end="")
    print(f"\", {scale_factor:.4}, {round(smallest)}); // average: {average:.0f}")

scaled = [v[0] * -160 for v in data["pst.weight"]]

dump_string(scaled[0:64], "0, PAWN")
dump_string(scaled[64:128], "0, KNIGHT")
dump_string(scaled[128:192], "0, BISHOP")
dump_string(scaled[192:256], "0, ROOK")
dump_string(scaled[256:320], "0, QUEEN")
dump_string(scaled[320:384], "0, KING")
dump_string(scaled[384:448], "1, PAWN")
dump_string(scaled[448:512], "1, KNIGHT")
dump_string(scaled[512:576], "1, BISHOP")
dump_string(scaled[576:640], "1, ROOK")
dump_string(scaled[640:704], "1, QUEEN")
dump_string(scaled[704:768], "1, KING")
dump_string(scaled[768:832], "0, KING+1")
dump_string(scaled[832:896], "1, KING+1")
