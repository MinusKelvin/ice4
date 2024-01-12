#!/usr/bin/python3

import math, sys, json
from contextlib import redirect_stdout
from collections import deque

def to_evalcpp(last_loss, train_id, param_map):
    print(f"// loss: {last_loss:.4g}    train id: {train_id}")
    print()
    print("#define S(a, b) (a + (b * 0x10000))")
    print()

    mg = deque(v * 160 for v in param_map["mg.weight"][0])
    eg = deque(v * 160 for v in param_map["eg.weight"][0])

    defines = []

    def define_param(name, *, sign=1):
        defines.append((
            name,
            round(mg.popleft()) * sign,
            round(eg.popleft()) * sign
        ))

    def array_param(name, size, *, leading_zero=False, sign=1):
        print(f"int {name}[] = {{", end="")
        if leading_zero:
            print("0, ", end="")
        print(", ".join(
            f"S({sign * round(mg.popleft())}, {sign * round(eg.popleft())})"
            for _ in range(size)
        ), end="")
        print("};")

    array_param("PIECE_RANK", 6*8)
    array_param("PIECE_FILE", 6*8)
    array_param("MOBILITY", 6, leading_zero=True)
    array_param("PASSED_PAWN", 8)
    define_param("ISOLATED_PAWN")
    define_param("DOUBLED_PAWN")
    define_param("KING_ATTACKS")
    define_param("DOUBLE_KING_ATTACKS")

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
