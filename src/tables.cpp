struct Nnue {
    float ft[768][NEURONS];
    float ft_bias[NEURONS];
    float out[NEURONS_X2];
    float out_bias;

    Nnue() {
        memset(this, 0, sizeof(Nnue));
    }
} NNUE;

int FEATURE[25][SQUARE_SPAN];

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
FILE *RNG_FILE;

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    RNG_FILE = fopen("/dev/urandom", "r");

    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST.pieces[i][j] = rng();
        }
    }
    for (int i = 0; i < 120; i++) {
        ZOBRIST.ep[i] = rng();
    }
    ZOBRIST.castle_rights[0] = rng();
    ZOBRIST.castle_rights[1] = rng();
    ZOBRIST.castle_rights[2] = rng();
    ZOBRIST.castle_rights[3] = rng();
    ZOBRIST.stm = rng();
#else
    fread(&ZOBRIST, sizeof(ZOBRIST), 1, RNG_FILE);
#endif

    // feature mapping
    int feature = 0;
    for (int p = PAWN; p <= KING; p++) {
        for (int file = 0; file < 8; file++) {
            for (int rank = 0; rank < 8; rank++) {
                FEATURE[p | WHITE][rank*10+file] = feature++;
                FEATURE[p | BLACK][70-rank*10+file] = feature++;
            }
        }
    }
}
