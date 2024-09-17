#!/usr/bin/python3

import math, sys, json
from contextlib import redirect_stdout
from collections import deque

class DataStringer:
    def __init__(self):
        self.data = ""

    def add(self, data, round_smallest=False):
        smallest = min(data)
        if round_smallest:
            smallest = round(smallest)
        for v in data:
            v = round(v - smallest)
            c = chr(v + 32)
            if v + 32 == 0x7F:
                self.data += "\\177"
            elif c == "\\" or c == "\"":
                self.data += "\\" + c
            else:
                self.data += c
        return smallest

def to_evalcpp(last_loss, train_id, param_map):
    print(f"// loss: {last_loss:.4g}    train id: {train_id}")
    print()
    print("#define S(a, b) (a + (b * 0x10000))")
    print()

    mg = deque(v * 160 for v in param_map["mg.weight"][0])
    eg = deque(v * 160 for v in param_map["eg.weight"][0])

    mg_stringer = DataStringer()
    eg_stringer = DataStringer()

    defines = []

    mg_off = round(mg_stringer.add([mg.popleft() for _ in range(48)]))
    eg_off = round(eg_stringer.add([eg.popleft() for _ in range(48)]))
    defines.append(("PAWN_OFFSET", mg_off, eg_off))

    mg_stringer.add([mg.popleft() for _ in range(16)])
    eg_stringer.add([eg.popleft() for _ in range(16)])

    print("int QUADRANTS[] = {", end="")
    for _ in range(4):
        mg_off = mg_stringer.add([mg.popleft() for _ in range(16)])
        eg_off = eg_stringer.add([eg.popleft() for _ in range(16)])
        mg_quad_offset = [0] + [mg.popleft() for _ in range(3)]
        eg_quad_offset = [0] + [eg.popleft() for _ in range(3)]
        for j, (mg_q, eg_q) in enumerate(zip(mg_quad_offset, eg_quad_offset)):
            if j % 4 == 0: print("\n   ", end="")
            print(f" S({round(mg_off + mg_q)}, {round(eg_off + eg_q)})", end=",")
    print("\n};")

    def define_param(name, *, coeff=1):
        defines.append((
            name,
            round(mg.popleft() * coeff),
            round(eg.popleft() * coeff)
        ))

    def array_param(name, size, *, leading_zero=False, coeff=1):
        print(f"int {name}[] = {{", end="")
        if leading_zero:
            print("0, ", end="")
        print(", ".join(
            f"S({round(mg.popleft() * coeff)}, {round(eg.popleft() * coeff)})"
            for _ in range(size)
        ), end="")
        print("};")

    define_param("BISHOP_PAIR")
    define_param("TEMPO", coeff=4)
    define_param("ISOLATED_PAWN", coeff=-1)
    define_param("PROTECTED_PAWN")
    define_param("ROOK_OPEN")
    define_param("ROOK_SEMIOPEN")
    array_param("PAWN_SHIELD", 4)
    define_param("KING_OPEN")
    define_param("KING_SEMIOPEN")
    array_param("MOBILITY", 6, leading_zero=True, coeff=4)
    define_param("KING_RING_ATTACKS", coeff=4)
    array_param("PASSER_RANK", 6)

    mg_off = mg_stringer.add([mg.popleft() for _ in range(16)], round_smallest=True)
    eg_off = eg_stringer.add([eg.popleft() for _ in range(16)], round_smallest=True)
    defines.append(("DIST_OFFSET", mg_off * 2, eg_off * 2))

    print()
    print(f"#define DATA_STRING L\"{mg_stringer.data + eg_stringer.data}\"")

    print()
    for name, mg, eg in defines:
        print(f"#define {name} S({mg}, {eg})")

def dump_result(result):
    with open("../src/eval.cpp", "w") as f:
        with redirect_stdout(f):
            to_evalcpp(result["loss"], result["train_id"], result["params"])

if __name__ == "__main__":
    with open(sys.argv[1]) as f:
        data = json.load(f)
    dump_result(data)
