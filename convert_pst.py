#!/usr/bin/python3

import json
with open("tune_state.json", "r") as f:
    data = json.load(f)

PIECES = ["PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING"]

def dump_strings(phase, data):
    for piece_data, piece in zip(data, PIECES):
        smallest = float("inf")
        largest = -smallest
        for rank in piece_data:
            for value in rank:
                smallest = smallest if smallest < value else value
                largest = largest if largest > value else value
        scale_factor = (largest - smallest) / 94
        print(f"unpack({phase}, {piece}, \"", end="")
        for rank in piece_data:
            for value in rank:
                c = chr(round((value - smallest) / scale_factor) + 32)
                if c == "\\" or c == "\"":
                    print("\\", end="")
                print(c, end="")
        print(f"\", {scale_factor:.4}, {round(smallest)});")

dump_strings(0, data["weights"]["mg"]["psts"])
dump_strings(1, data["weights"]["eg"]["psts"])
