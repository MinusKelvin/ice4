# ice4

![ice4 logo](logo.png?raw=true)

ice4 is a chess engine which fits in 4096 bytes.
In particular, it is an executable shell script which automatically extracts C++ source code from itself using `xz`, compiles it using the default system C++ compiler, and executes the resulting binary.

It supports the following subset of UCI:
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
- `go infinite` (`stop` is additionally supported for this form)
- `wtime` and `btime` are not subject to the location requirement mentioned above.

## Features

### High-level strategy

- 10x12 board representation
- Pseudolegal move generation
  - Underpromotions are not generated
- Twofold repetition detection
- Zobrist hashing
- Lazy SMP
  - Best move selected by depth tiebroken by score
- Iterative deepening
- Soft limit + hard limit time management
- Principal variation search
- Aspiration windows
  - Gradually widened
  - Results used if exact or fail-high
- Incremental evaluation

### Search

- Transposition table cutoffs
- Quiescence search
- Internal Iterative Reductions
- Improving
- Reverse Futility Pruning
- Null Move Pruning
- Razoring
- Internal Iterative Deepening
- Pawn Protected Pruning
- Late Move Pruning
- Delta Pruning
- Check Extensions
- Late Move Reductions
  - Logarithmic base formula based on move index and depth
  - Increased reduction if TT move is capture
  - Adjusted based on move history
- Correction Histories
  - Updated using rolling average formula with squared depth 
  - Pawn Correction History
  - Material Correction History
  - Countermove Correction History
  - Followup Correction History
- Move Ordering
  - TT move
  - MVV-LVA
  - Move Histories
    - Updated using gravity formula with squared depth
    - Plain History
    - Continuation History
    - Followup History

### Evaluation

- King-relative pawn square table
- Piece rank tables for non-pawns
- Piece file tables for non-pawns
- Tempo
- Mobility (approximated)
- Bishop pair
- Rook on open file
- Rook on semi-open file
- Additional pawn evaluation
  - Isolated pawn
  - Protected pawn
  - Phalanx pawn rank table
  - Passed pawn rank table
  - Passed pawn distance to own king table
  - Passed pawn distance to enemy king table
- King safety
  - King on open file
  - King on semi-open file
  - Shield pawn count table
  - King ring attacks (approximated)

For performance reasons, mobility and king ring attack terms for the side not to move are approximated by using the value from the previous ply.

## Acknowledgements

- This follows the TCEC 4K rules: https://wiki.chessdom.org/TCEC_4k_Rules
- Thanks to Analog for their many contributions to ice4, including the name and logo
