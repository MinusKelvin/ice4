# ice4

ice4 is a chess engine which fits in 4096 bytes.
In particular, it is an executable text file containing a script which automatically extracts C source code from itself using `xz`, compiles it using the default system C compiler, and then executes it.

It support the following subset of UCI:
- `uci` (only as first message, required to be first message)
- `isready`
- `position startpos moves ...`
- `go wtime <#> btime <#> ...`

Additionally, OpenBench builds (`make EXE=<name>`) support `position fen` and `setoption` for hash and threads.

## Features

- A single executable no more than 4 KB large (notable dependencies: C++ compiler, `xz`)
- 10x12 board representation
- Pseudolegal movegen
- Zobrist hashing
- Transposition Table
- Twofold repetition detection
- Principle Variation Search
  - Quiescense search
  - Null Move Pruning
- Move Ordering
  - TT move
  - MVV-LVA captures before quiets
  - History
- Iterated Deepening
- Soft limit + hard limit time management
- Evaluation
  - Piece Square Tables (courtesy of Analog)

## Acknowledgements

- This follows the TCEC 4K rules: https://wiki.chessdom.org/TCEC_4k_Rules
- Thanks to Analog for coming up with the name and providing piece square tables
