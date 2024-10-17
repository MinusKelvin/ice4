# ice4

![ice4 logo](logo.png?raw=true)

ice4 is a chess engine which fits in 4096 bytes.
In particular, it is an executable shell script which automatically extracts C++ source code from itself using `xz`, compiles it using the default system C++ compiler, and executes the resulting binary.

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

OpenBench builds (`make EXE=<name>` or `make ice4-ob`) support some additional UCI features:
- `position fen <fen> moves ...`
- `setoption name Hash value <mb>`
- `setoption name Threads value <threads>`
- `go infinite` (the next message will be interpreted as `stop`)

## Features

- A single executable no more than 4 KiB large (notable dependencies: C++ compiler, `xz`)
- 10x12 board representation
- Pseudolegal movegen
  - Underpromotions to rook & bishop not generated
- Zobrist hashing
- Transposition table
- Lazy SMP
- Twofold repetition detection
- Principal variation search
  - Quiescence search
  - Delta pruning
  - Null move pruning
  - Reverse futility pruning
  - Internal iterative deepening
  - Internal iterative reductions
  - Late move reductions
  - Late move pruning
  - History reductions
  - Check Extensions
- Move Ordering
  - TT move
  - MVV-LVA captures before quiets
  - History
  - Countermove History
- Iterative deepening
- Soft limit + hard limit time management
- Evaluation
  - Phased evaluation with 418 parameters
  - Quadrant piece square tables for pieces
  - Shrunken piece square table for king
  - King-relative piece square tables for pawns and passed pawns
  - Doubled pawns
  - Isolated pawns
  - Simplified pawn chain
  - Rook on (semi-)open files
  - Bishop pair
  - Tempo
  - Basic king safety (open file, pawn shield)

## Acknowledgements

- This follows the TCEC 4K rules: https://wiki.chessdom.org/TCEC_4k_Rules
- Thanks to Analog for coming up with the name, logo, and various other contributions
