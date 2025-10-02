#!/usr/bin/python3

import math, sys, json
from contextlib import redirect_stdout
from collections import deque

class DataStringer:
    def __init__(self):
        self.data = ""
        self.len = 0

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
        self.len += len(data)
        return smallest

def to_evalcpp(last_loss, train_id, param_map):
    print(f"// loss: {last_loss:.4g}    train id: {train_id}")
    print()
    print("#define S(a, b) (a + (b * 0x10000))")
    print()

    mg = deque(v * 120 for v in param_map["mg.weight"][0] + param_map["king_attack.weight"][0])
    eg = deque(v * 120 for v in param_map["eg.weight"][0] + [0] * len(param_map["king_attack.weight"][0]))

    mg_stringer = DataStringer()
    eg_stringer = DataStringer()

    defines = []

    def define_param(name, *, sign=1):
        defines.append((
            name,
            round(mg.popleft()) * sign,
            round(eg.popleft()) * sign
        ))

    def array_param(name, size, *, leading_zero=False, trailing_zeros=0, sign=1):
        print(f"int {name}[] = {{", end="")
        if leading_zero:
            print("0, ", end="")
        print(", ".join(
            f"S({sign * round(mg.popleft())}, {sign * round(eg.popleft())})"
            for _ in range(size)
        ), end="")
        print(", 0" * trailing_zeros, end="")
        print("};")

    def datastring_param(name, size, *, adjust=0):
        defines.append((f"{name}_INDEX", mg_stringer.len + adjust))
        mg_off = mg_stringer.add([mg.popleft() for _ in range(size)], round_smallest=True)
        eg_off = eg_stringer.add([eg.popleft() for _ in range(size)], round_smallest=True)
        defines.append((name, mg_off, eg_off))

    print("int MATERIAL[] = {0", end="")
    mg_off = mg_stringer.add([mg.popleft() for _ in range(48)], round_smallest=True)
    eg_off = eg_stringer.add([eg.popleft() for _ in range(48)], round_smallest=True)
    print(f", S({mg_off}, {eg_off})", end="")
    for i in range(5):
        mg_off = mg_stringer.add([mg.popleft() for _ in range(8)], round_smallest=True)
        mg_off += mg_stringer.add([mg.popleft() for _ in range(8)], round_smallest=True)
        eg_off = eg_stringer.add([eg.popleft() for _ in range(8)], round_smallest=True)
        eg_off += eg_stringer.add([eg.popleft() for _ in range(8)], round_smallest=True)
        if i == 4: mg_off = eg_off = 0
        print(f", S({mg_off}, {eg_off})", end="")
    print("};")

    define_param("BISHOP_PAIR")
    define_param("TEMPO")
    define_param("ISOLATED_PAWN", sign=-1)
    define_param("PROTECTED_PAWN")
    define_param("ROOK_OPEN")
    define_param("ROOK_SEMIOPEN")
    datastring_param("PAWN_SHIELD", 4)
    define_param("KING_OPEN")
    define_param("KING_SEMIOPEN")
    datastring_param("MOBILITY", 6, adjust=-1)
    datastring_param("PASSER_RANK", 6, adjust=-1)
    datastring_param("OWN_KING_PASSER_DIST", 8)
    datastring_param("OPP_KING_PASSER_DIST", 8)
    datastring_param("PHALANX_RANK", 6, adjust=-1)

    array_param("KING_ATTACK_WEIGHT", 5, leading_zero=True, trailing_zeros=1)
    define_param("ATTACKER_NO_QUEEN")

    print()
    print(f"#define DATA_STRING L\"{mg_stringer.data + eg_stringer.data}\"")
    print()
    print(f"#define EG_OFFSET {mg_stringer.len}")

    print()
    for name, *data in defines:
        if len(data) == 2:
            mg, eg = data
            print(f"#define {name} S({mg}, {eg})")
        else:
            print(f"#define {name} {data[0]}")

def dump_result(result):
    with open("../src/eval.cpp", "w") as f:
        with redirect_stdout(f):
            to_evalcpp(result["loss"], result["train_id"], result["params"])

if __name__ == "__main__":
    with open(sys.argv[1]) as f:
        data = json.load(f)
    dump_result(data)
