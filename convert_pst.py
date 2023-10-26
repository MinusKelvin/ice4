#!/usr/bin/python3

import json, math
with open("0-20.json", "r") as f:
    data = json.load(f)

class DataStringer:
    def __init__(self):
        self.little = ""
        self.big = ""

    def add(self, data):
        smallest = min(data)
        for v in data:
            v = round(v - smallest)
            low = v % 95
            high = v // 95
            lc = chr(low + 32)
            hc = chr(high + 32)
            if lc == "\\" or lc == "\"":
                self.little += "\\"
            if hc == "\\" or hc == "\"":
                self.big += "\\"
            self.little += lc
            self.big += hc
        return smallest

def dump_string(piece_data, stuff, extra=None):
    smallest = float("inf")
    largest = -smallest
    average = 0
    for value in piece_data:
        smallest = smallest if smallest < value else value
        largest = largest if largest > value else value
        average += value / len(piece_data)

    scale_factor = max((largest - smallest) / 94, 1.0)

    s = ""
    for value in piece_data:
        c = chr(round((value - smallest) / scale_factor) + 32)
        if c == "\\" or c == "\"":
            s += "\\"
        s += c

    print(f"{stuff}, {scale_factor:.4}, {round(smallest)}", end="")
    if extra:
        for v in extra:
            print(f", {round(smallest + v)}", end="")
    print(f"); // average: {average:.0f}")

    return s

scaled = [v * 160 for v in data["params.weight"][0]]

sections = []
sizes = [48, 16, 3, 16, 3, 16, 3, 16, 3, 16, 48, 1, 8, 1, 1, 1, 1, 1, 1, 4, 1, 1, 1] * 2
acc = 0
for s in sizes:
    sections.append(scaled[acc:acc+s])
    acc += s

eg = len(sections)//2

data_string = ""

mg_stringer = DataStringer()
eg_stringer = DataStringer()

mg_off = round(mg_stringer.add(sections[0]))
eg_off = round(eg_stringer.add(sections[0+eg]))
print(f"#define PAWN_OFFSET S({mg_off}, {eg_off})")
mg_off = round(mg_stringer.add(sections[10]))
eg_off = round(eg_stringer.add(sections[10+eg]))
print(f"#define PASSED_PAWN_OFFSET S({mg_off}, {eg_off})")

mg_stringer.add(sections[9])
eg_stringer.add(sections[9+eg])

print("int QUADRANTS[] = {", end="")
for i in range(1, 9, 2):
    mg_off = mg_stringer.add(sections[i])
    eg_off = eg_stringer.add(sections[i+eg])
    for j, (mg_q, eg_q) in enumerate(zip([0] + sections[i+1], [0] + sections[i+1+eg])):
        if j % 4 == 0: print("\n   ", end="")
        print(f" S({round(mg_off + mg_q)}, {round(eg_off + eg_q)})", end=",")
print("\n};")

print(f"#define BISHOP_PAIR S({round(sections[11][0])}, {round(sections[eg+11][0])})")
print("int DOUBLED_PAWN[] = {" + ", ".join(
    f"S({-round(v1)}, {-round(v2)})" for v1, v2 in zip(sections[12], sections[eg+12])
) + "};")
print(f"#define TEMPO S({round(sections[13][0])}, {round(sections[eg+13][0])})")
print(f"#define ISOLATED_PAWN S({-round(sections[14][0])}, {-round(sections[eg+14][0])})")
print(f"int PROTECTED_PAWN[] = {{0, S({round(sections[15][0])}, {round(sections[eg+15][0])}), S({round(sections[16][0])}, {round(sections[eg+16][0])})}};")
print(f"#define ROOK_OPEN S({round(sections[17][0])}, {round(sections[eg+17][0])})")
print(f"#define ROOK_SEMIOPEN S({round(sections[18][0])}, {round(sections[eg+18][0])})")
print("int PAWN_SHIELD[] = {" + ", ".join(
    f"S({round(v1)}, {round(v2)})" for v1, v2 in zip(sections[19], sections[eg+19])
) + "};")
print(f"#define KING_OPEN S({round(sections[20][0])}, {round(sections[eg+20][0])})")
print(f"#define KING_SEMIOPEN S({round(sections[21][0])}, {round(sections[eg+21][0])})")
print(f"#define PAWN_NEIGHBOR S({round(sections[22][0])}, {round(sections[eg+22][0])})")

print(f"data string low: \"{mg_stringer.little + eg_stringer.little}\"")
print(f"data string high: \"{mg_stringer.big + eg_stringer.big}\"")
