#!/usr/bin/python3

import json, math
from scipy.optimize import least_squares
import numpy as np

with open("0-45.json", "r") as f:
    data = json.load(f)

def quantize(vector):
    smallest = vector.min()
    largest = vector.max()
    scale_factor = 94 / (largest - smallest)
    values = np.around((vector - smallest) * scale_factor)
    unquantized = values / scale_factor + smallest
    return smallest, scale_factor, values, unquantized

for f, r in zip(data["files"], data["ranks"]):
    v = np.concatenate((np.array(f), np.array(r))) * -12.65
    smallest, scale, quant, unquant = quantize(v)

    print(f"unpack({smallest:.4}, {1/scale:.4}, \"", end="")
    for v in quant:
        c = chr(int(v) + 32)
        if c == "\"" or c == "\\":
            print("\\", end="")
        print(c, end="")
    print("\");")
