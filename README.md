# ice4

![ice4 logo](logo.png?raw=true)

ice4 is a chess engine which fits in 4096 bytes.
In particular, it is an executable text file containing a script which automatically extracts C source code from itself using `xz`, compiles it using the default system C compiler, and then executes it.

It support the following subset of UCI:
- `uci`
- `ucinewgame`
- `isready`
- `position startpos moves ...`
- `go wtime <#> btime <#> ...`
- `quit`

This subset is implemented in a space-efficient manner and so has some limitations.
The first message is always interpreted as `uci` and none of the following messages may be interpreted as `uci` (following `uci` messages will be interpreted as `ucinewgame` instead).
The `wtime` and `btime` parameters to the `go` command must be present immediately after `go` and in that order.
This location requirement may limit GUI compatibility.
Only Cute Chess is known to work.

Additionally, OpenBench builds (`make EXE=<name>`) support `position fen` and `setoption` for setting the hash size and thread count (threading currently not implemented).

## Features

- A single executable no more than 4 KB large (notable dependencies: C++ compiler, `xz`)
- 10x12 board representation
- Pseudolegal movegen
  - No castling movegen (castling by opponent supported)
- Zobrist hashing
- Transposition table
- Twofold repetition detection
- Principal variation search
  - Quiescense search
  - Null move pruning
  - Reverse futility pruning
  - Internal iterated deepening
- Move Ordering
  - TT move
  - MVV-LVA captures before quiets
  - History
- Iterated deepening
- Soft limit + hard limit time management
- Evaluation
  - Phased piece square tables

## Acknowledgements

- This follows the TCEC 4K rules: https://wiki.chessdom.org/TCEC_4k_Rules
- Thanks to Analog for coming up with the name and providing piece square tables
