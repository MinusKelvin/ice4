#!/usr/bin/python3

import json, math
with open("0-10.json", "r") as f:
    data = json.load(f)

print("FT weights:", [v for l in data["ft.weight"] for v in l])
print("FT bias:", data["ft.bias"])
print("out weights:", data["out.weight"][0])
print("out bias:", data["out.bias"][0])
