#!/usr/bin/python3

import json, math
with open("0-23.json", "r") as f:
    data = json.load(f)

print("FT weights:", [data["ft.weight"][i][j] for j in range(768) for i in range(16)])
print("FT bias:", data["ft.bias"])
print("out weights:", data["out.weight"][0])
print("out bias:", data["out.bias"][0])
