#!/usr/bin/python3

import json, math
from scipy.optimize import least_squares
import numpy as np

with open("0-45.json", "r") as f:
    data = json.load(f)

def quantize(vector):
    smallest = vector.min()
    largest = vector.max()
    scale_factor = 94 / (largest - smallest) if largest > smallest else 1
    values = np.around((vector - smallest) * scale_factor)
    unquantized = values / scale_factor + smallest
    return smallest, scale_factor, values, unquantized

for i, (v, vg, f, fg) in enumerate(zip(data["ft.weight"], data["ft.gate"], data["material_fact.weight"], data["material_fact.gate"])):
    v = np.array(v) * (np.array(vg) > 0.5)
    f = (np.array(f) * (np.array(fg) > 0.5)).reshape((12, 1))
    v = v + np.pad(f, ((0, 0), (0, 63)), "edge").ravel()
    smallest, scale, quant, unquant = quantize(v * 256)

    print(f"unpack({i}, {smallest:.4}, {1/scale:.4}, \"", end="")
    for v in quant:
        c = chr(int(v) + 32)
        if c == "\"" or c == "\\":
            print("\\", end="")
        print(c, end="")
    print("\");")

for v in data["ft.bias"]:
    print(f"{round(v*256)}, ", end="")
print()

for v in data["out.weight"][0]:
    print(f"{round(v*64)}, ", end="")
print()

print(f"{round(data['out.bias'][0]*256*64)}")
