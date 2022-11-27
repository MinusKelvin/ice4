#!/usr/bin/python3

def process_data(data, bias, intrinsic_scale):
    # Identify data-relative scaling factor
    small = [float("inf")] * len(data[0])
    large = [-float("inf")] * len(data[0])
    for vector in data:
        for i, v in enumerate(vector):
            small[i] = min(small[i], v)
            large[i] = max(large[i], v)
    scale = [94 / (l - s) for l, s in zip(large, small)]

    # Emit data string
    print("\"", end="")
    for vector in data:
        for i, v in enumerate(vector):
            v = chr(round((v - small[i]) * scale[i]) + 32)
            if v == "\"" or v == "\\":
                print("\\", end="")
            print(v, end="")
    print("\"")

    # Emit decoding parameters
    print("scale: ", end="")
    for v in scale:
        print(f"{intrinsic_scale / v:.4}, ", end="")
    print()
    print("offsets: ", end="")
    for v in small:
        print(f"{v * intrinsic_scale:.4}, ", end="")
    print()
    print("bias: ", end="")
    for w in bias:
        print(f"{round(w * intrinsic_scale)}, ",end="")
    print()

ACTIVATION_RANGE = 127
OUT_SCALE = 64

import json, math
with open("filenetwork.json", "r") as f:
    data = json.load(f)

print("FEATURE TRANSFORMER")
process_data(data["ft.weight"], data["ft.bias"], ACTIVATION_RANGE)

print("OUTPUT LAYER")
process_data(data["out.weight"], [data["out.bias"][0] * ACTIVATION_RANGE], OUT_SCALE)

pst = [[] for i in range(32)]
for i in range(32):
    for j in range(0, 384, 32):
        pst[i].append(data["pst"][i+j][0])

process_data(pst, [0]*12, ACTIVATION_RANGE * OUT_SCALE / 40)
