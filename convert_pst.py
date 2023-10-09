#!/usr/bin/python3

import json
from math import cos, sqrt, pi, copysign
with open("dct-psts.json", "r") as f:
    data = json.load(f)

def dump_string(data):
    smallest = min(data)
    largest = max(data)

    scale_factor = (largest - smallest) / 94

    s = ""
    quantized = []
    for value in data:
        v = round((value - smallest) / scale_factor)
        quantized.append(v * scale_factor + smallest)
        c = chr(v + 32)
        if c == "\\" or c == "\"":
            s += "\\"
        s += c

    return s, smallest, scale_factor, quantized

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
    for i in [0, 1, 2, 3, 4, 5, 6, eg+0, eg+1, eg+2, eg+3, eg+4, eg+5, eg+6]
]

dct_psts = [dct2(pst) for pst in psts]

freq = []
for i, pst in enumerate(dct_psts):
    for r, rank in enumerate(pst):
        for f, value in enumerate(rank):
            freq.append((i, r * 8 + f, copysign(sqrt(abs(value)), value)))
freq.sort(key=lambda x: abs(x[2]), reverse=True)
freq = freq[:896]

freq.sort()
boards = [b for b, _, _ in freq]
frequencies = [f for _, f, _ in freq]
amplitudes = [a for _, _, a in freq]

print(boards)
print(frequencies)
data_string, smallest, scale, quantized = dump_string(amplitudes)

print(quantized)

print(f"amplitudes: \"{data_string}\"")
print(f"smallest, scale: {smallest:.4}, {scale:.4}")

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
