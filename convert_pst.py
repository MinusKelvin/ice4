#!/usr/bin/python3

import json, sys
from math import cos, sqrt, pi, copysign
from scipy.optimize import least_squares
import numpy as np

with open("dct-psts.json", "r") as f:
    data = json.load(f)

def stringdata(data):
    s = ""
    for v in data:
        if v + 32 not in range(32, 127):
            print()
        c = chr(v + 32)
        if c == "\\" or c == "\"":
            s += "\\"
        s += c
    return s

def dump_string(data):
    smallest = min(data)
    largest = max(data)

    scale_factor = (largest - smallest) / 94

    s = stringdata(map(lambda value: round((value - smallest) / scale_factor), data))

    return s, smallest, scale_factor

def dct2(x):
    return [
        [
            sum(
                x[n1][n2] * cos((2 * n1 + 1) * k1 * pi / 2 / len(x)) * cos((2 * n2 + 1) * k2 * pi / 2 / len(x[n1])) / 2**((k1 == 0) + (k2 == 0)) * 4 / len(x) / len(x[0])
                for n1 in range(len(x))
                for n2 in range(len(x[n1]))
            )
            for k2 in range(len(x[k1]))
        ]
        for k1 in range(len(x))
    ]

def idct2(x):
    return [
        [
            sum(
                x[k1][k2] * cos((2 * n1 + 1) * k1 * pi / 2 / len(x)) * cos((2 * n2 + 1) * k2 * pi / 2 / len(x[n1]))
                for k1 in range(len(x))
                for k2 in range(len(x[k1]))
            )
            for n2 in range(len(x[n1]))
        ]
        for n1 in range(len(x))
    ]

scaled = [v * 160 for v in data["params.weight"][0]]

sections = []
sizes = [64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0, 1, 8, 1, 1, 1, 1, 1, 1, 4, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

psts = [
    [
        sections[i][sq:sq+8]
        for sq in range(0, 64, 8)
    ]
    for i in [0, eg+0, 1, eg+1, 2, eg+2, 3, eg+3, 4, eg+4, 5, eg+5, 6, eg+6]
]

dct_psts = [dct2(pst) for pst in psts]

freq = []
for i, pst in enumerate(dct_psts):
    for r, rank in enumerate(pst):
        for f, value in enumerate(rank):
            freq.append((i*64 + r * 8 + f, value))
freq.sort(key=lambda x: abs(x[1]), reverse=True)
freq = freq[:]

freq.sort()
lossy_dct_psts = [[[0] * 8 for _ in range(8)] for _ in range(len(dct_psts))]
for i, v in freq:
    lossy_dct_psts[i // 64][i % 64 // 8][i % 8] = v

compressed_psts = [idct2(pst) for pst in lossy_dct_psts]
print(f"lossy error: {(np.array(compressed_psts) - np.array(psts)).std()}")

def pick(palette, x):
    palette = [p * r for p in palette[:8] for r in palette[8:]]
    i = min(range(64), key=lambda i: abs(palette[i] - x))
    return i, palette[i]

def residuals(palette):
    residual = []
    for pst in lossy_dct_psts:
        for i, r in enumerate(pst):
            for j, v in enumerate(r):
                if (i != 0 or j != 0) and v != 0:
                    residual.append(v - pick(palette, v)[1])
    return residual

soln = least_squares(residuals, list(range(8)) + [1] * 8)

amplitudes = []
for i, v in freq:
    if i % 64 == 0:
        lossy_dct_psts[i // 64][i % 64 // 8][i % 8] = v
    else:
        j, v = pick(soln.x, v)
        lossy_dct_psts[i // 64][i % 64 // 8][i % 8] = v
        amplitudes.append(j)
amplitudes_string = stringdata(amplitudes)

compressed_psts = [idct2(pst) for pst in lossy_dct_psts]
print(f"quantized error: {(np.array(compressed_psts) - np.array(psts)).std()}")

frequencies = [f for f, _ in freq]
skips_string = stringdata(f1 - f2 - 1 for f1, f2 in zip(frequencies[1:], frequencies))

# print(boards)

# print(quantized)

print(f"amplitudes: \"{amplitudes_string}\"")
print(f"skips: \"{skips_string}\"")
print(f"base palette: {soln.x[:8]}")
print(f"residual palette: {soln.x[8:]}")

print(f"material palette: {[round(pst[0][0]) for pst in lossy_dct_psts]}")

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
