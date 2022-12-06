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

groups = []
for i in range(12):
    values = np.array(data["pst.weight"][i*64:i*64+64]).ravel() * -160
    if i == 0 or i == 6:
        values[0:8] = np.median(values)
        values[56:64] = np.median(values)
    values = np.around(values.reshape((8,8)))
    print(int(np.amin(values)), end=", ")
    values -= np.amin(values)
    groups.append(values)
print()

def model(x, group):
    files = x[0:48]
    ranks = x[48:64]
    pst = np.outer(files, ranks).ravel()
    residuals = group - pst
    return residuals

pawns = np.concatenate((groups[0], groups[6]), axis=1)
knights = np.concatenate((groups[1], groups[7]), axis=1)
bishops = np.concatenate((groups[2], groups[8]), axis=1)
rooks = np.concatenate((groups[3], groups[9]), axis=1)
queens = np.concatenate((groups[4], groups[10]), axis=1)
kings = np.concatenate((groups[5], groups[11]), axis=1)

target = np.concatenate((pawns, knights, bishops, rooks, queens, kings), axis=0).ravel()
remaining = target
predicted = np.zeros_like(remaining)
rng = np.random.default_rng()

ITERS = 8

for i in range(ITERS):
    result = least_squares(model, rng.uniform(-1, 1, 64), args=(remaining,))
    smallest, scale, quant, unquant = quantize(result.x)

    print(f"unpack({smallest:.4}, {1/scale:.4}, \"", end="")
    for v in quant:
        c = chr(int(v) + 32)
        if c == "\"" or c == "\\":
            print("\\", end="")
        print(c, end="")
    print("\");", end="")

    prediction = np.outer(unquant[0:48], unquant[48:64]).ravel()
    predicted += prediction
    predicted = np.trunc(predicted)
    remaining = target - predicted

    print(f" // mse = {np.mean(np.square(remaining)):.4}")
