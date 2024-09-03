#!/usr/bin/python3

import math, sys, json
from contextlib import redirect_stdout
from collections import deque

class DataStringer:
    def __init__(self):
        self.data = ""

    def add(self, data):
        smallest = min(data)
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

    def define_param(name, *, sign=1):
        defines.append((
            name,
            round(mg.popleft()) * sign,
            round(eg.popleft()) * sign
        ))

    def array_param(name, *sizes, leading_zero=False, sign=1):
        print(f"int {name}[]", end="")
        for s in sizes[1:]:
            print(f"[{s}]", end="")
        print(" = ", end="")
        def print_arr(sizes):
            if len(sizes) == 0:
                print(f"S({sign * round(mg.popleft())}, {sign * round(eg.popleft())})", end="")
            else:
                print("{ ", end="")
                if leading_zero and len(sizes) == 1:
                    print("0, ", end="")
                for i in range(sizes[0]):
                    if i != 0: print(", ", end="")
                    print_arr(sizes[1:])
                print(" }", end="")
        print_arr(sizes)
        print(";")

    define_param("BISHOP_PAIR")
    define_param("TEMPO")
    define_param("ISOLATED_PAWN", sign=-1)
    define_param("PROTECTED_PAWN")
    define_param("ROOK_OPEN")
    define_param("ROOK_SEMIOPEN")
    array_param("PAWN_SHIELD", 4)
    define_param("KING_OPEN")
    define_param("KING_SEMIOPEN")
    array_param("MOBILITY", 6, leading_zero=True)
    define_param("KING_RING_ATTACKS")
    array_param("PASSER_RANK", 2, 6)

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
