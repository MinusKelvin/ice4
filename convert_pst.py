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

for i, (f, r) in enumerate(zip(data["files"], data["ranks"])):
    v = np.concatenate((np.array(f), np.array(r))) * 16
    smallest, scale, quant, unquant = quantize(v)

    print(f"unpack({i}, {smallest:.4}, {1/scale:.4}, \"", end="")
    for v in quant:
        c = chr(int(v) + 32)
        if c == "\"" or c == "\\":
            print("\\", end="")
        print(c, end="")
    print("\");")

for v in data["bias"]:
    print(f"{round(v*256)}, ", end="")
print()

for v in data["out.weight"][0]:
    print(f"{round(v*64)}, ", end="")
print()

print(f"{round(data['out.bias'][0]*256*64)}")
