// 8MB. Replaced for TCEC builds by the minifier.
#define HASH_SIZE 524288

#ifdef OPENBENCH
int THREADS = 1;
#else
// Replaced for TCEC builds by the minifier.
#define THREADS 1
#endif

#define DATAGEN_DEPTH 5
#define DATAGEN_SIZE 1000000
#define OUTCOME_PART 1.0
#define EVAL_PART 0.0
#define EVAL_SCALE 203.0
#define NEURONS 16
#define NEURONS_X2 32
#define FT_INIT_SCALE 0.036
#define OUT_INIT_SCALE 0.25
#define BATCH_SIZE 4096
#define FEATURE_FLIP 1

#define LOST -30000
#define WON 30000

#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE 8
#define BLACK 16

#define INVALID 24

#define SHORT_CASTLE 1
#define LONG_CASTLE 2

#define WHITE_PAWN 9
#define BLACK_PAWN 17
#define PASSED_PAWN 7
#define WHITE_PASSED_PAWN 15
#define BLACK_PASSED_PAWN 23

#define SQUARE_SPAN 78

#define BOUND_EXACT 0
#define BOUND_LOWER 1
#define BOUND_UPPER 2

#define A1 21
#define B1 22
#define C1 23
#define D1 24
#define E1 25
#define F1 26
#define G1 27
#define H1 28
#define A2 31
#define B2 32
#define C2 33
#define D2 34
#define E2 35
#define F2 36
#define G2 37
#define H2 38
#define A3 41
#define B3 42
#define C3 43
#define D3 44
#define E3 45
#define F3 46
#define G3 47
#define H3 48
#define A4 51
#define B4 52
#define C4 53
#define D4 54
#define E4 55
#define F4 56
#define G4 57
#define H4 58
#define A5 61
#define B5 62
#define C5 63
#define D5 64
#define E5 65
#define F5 66
#define G5 67
#define H5 68
#define A6 71
#define B6 72
#define C6 73
#define D6 74
#define E6 75
#define F6 76
#define G6 77
#define H6 78
#define A7 81
#define B7 82
#define C7 83
#define D7 84
#define E7 85
#define F7 86
#define G7 87
#define H7 88
#define A8 91
#define B8 92
#define C8 93
#define D8 94
#define E8 95
#define F8 96
#define G8 97
#define H8 98
