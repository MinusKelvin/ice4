int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {10, -13, 18, 15, 28, 9, -13, 20};
int16_t DOUBLED_EG[] = {30, 17, 7, 9, 5, 12, 16, 33};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 8};
int16_t PROTECTED_PAWN_EG[] = {0, 6, 4};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 12
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 18
#define ROOK_SEMIOPEN_EG 13

void unpack_full(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[1+rank+file] = v;
            black_section[70-rank+file] = -v;
            black_section[71-rank+file] = -v;
        }
    }
}

void unpack_half(
    int phase, int piece, const char *data, double scale, int qll, int qlr, int qrl, int qrr
) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale;
            white_section[rank+file] = v + qll;
            white_section[7+rank-file] = v + qrl;
            white_section[70-rank+file] = v + qlr;
            white_section[77-rank-file] = v + qrr;
            black_section[rank+file] = -v - qlr;
            black_section[7+rank-file] = -v - qrr;
            black_section[70-rank+file] = -v - qll;
            black_section[77-rank-file] = -v - qrl;
        }
    }
}

#ifdef OPENBENCH
// Deterministic PRNG for openbench build consistency
uint32_t rng_32() {
    static uint64_t RNG_STATE = 0xcafef00dd15ea5e5;
    // Pcg32
    uint64_t old = RNG_STATE;
    RNG_STATE = old * 6364136223846793005ull + 0xa02bdbf7bb3c0a7ull;
    uint32_t xorshifted = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
uint64_t rng() {
    return (uint64_t) rng_32() << 32 | rng_32();
}
#endif

uint64_t ZOBRIST_PIECES[23][SQUARE_SPAN];
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    unpack_full(0, PAWN, "    LO9.LH=0HVL4OcYAbzkL li;    ", 1.0, 20); // average: 53
    unpack_full(1, PAWN, "    &41- *'%%$$)-*-2::=<Lz~z    ", 1.267, 92); // average: 116
    unpack_full(0, PASSED_PAWN, "    07,1/' 2/#%:414C-3BSdHe~    ", 1.169, -14); // average: 9
    unpack_full(1, PASSED_PAWN, "    & '),()+:5/6I=4A]I<Q~L2H    ", 1.84, -9); // average: 28
    unpack_full(0, KING, "D:;L=+/A- !+*(%#573,:77/@>5)~y_\\", 1.92, -52); // average: -2
    unpack_full(1, KING, "&(! 7HF2FVTEP_^O`ij]syyru~}w>WYH", 1.138, -57); // average: 3
    unpack_half(0, QUEEN, " ')-./4444522.1.", 1.0, 635, 636, 639, 655); // average: 650
    unpack_half(1, QUEEN, "9.% +1-2.DLK3LWd", 1.0, 1273, 1329, 1266, 1330); // average: 1300
    unpack_half(0, ROOK, "017A %-2,4/3+%-2", 1.0, 276, 310, 282, 321); // average: 291
    unpack_half(1, ROOK, " '&!'*((#&)&&/-)", 1.0, 629, 655, 623, 647); // average: 636
    unpack_half(0, BISHOP, "\" !!0200+<8822=B", 1.0, 235, 241, 233, 249); // average: 251
    unpack_half(1, BISHOP, " 0&.)5864=DF,?EJ", 1.0, 368, 378, 369, 377); // average: 390
    unpack_half(0, KNIGHT, " %*.*)37*?>D;7IE", 1.0, 219, 239, 223, 244); // average: 240
    unpack_half(1, KNIGHT, " ATXK]chVhqx_py~", 1.236, 266, 277, 266, 277); // average: 344
    
    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 23; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST_PIECES[i][j] = rng();
        }
    }
    ZOBRIST_CASTLE_RIGHTS[0] = rng();
    ZOBRIST_CASTLE_RIGHTS[1] = rng();
    ZOBRIST_CASTLE_RIGHTS[2] = rng();
    ZOBRIST_CASTLE_RIGHTS[3] = rng();
    ZOBRIST_STM = rng();
#else
    auto rng = fopen("/dev/urandom", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
