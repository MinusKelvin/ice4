double SQRT[64];
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int STARTS[] = {0,0,8,4,0,0,0};
int LIMITS[] = {0,0,1,8,8,8,1};
int ENDS[] = {0,0,16,8,4,8,8};
int SLIDER[] = {ROOK, ROOK, ROOK, ROOK, BISHOP, BISHOP, BISHOP, BISHOP, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT};
int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
int DELTAS[] = {717, 100, 423, 569, 755, 1280, 0};

int get_data(int i) {
    auto data = DATA_STRING;
    return data[i] + 0x10000 * data[i+144] - S(32, 32);
}

#ifdef OPENBENCH
// Deterministic PRNG for openbench build consistency
uint64_t RNG_STATE = 0xcafef00dd15ea5e5;
uint32_t rng_32() {
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

uint64_t ZOBRIST[25][SQUARE_SPAN];

void init_tables() {
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            SQRT[rank*8+file] = sqrt(rank*8+file);

            PST[BLACK | KING][70-10*rank+file] = -(
                PST[WHITE | KING][10*rank+file] = get_data(rank/2*4+file/2+48)
            );

            if (rank > 0 && rank < 7) {
                PST[WHITE_PAWN][10*rank+file] = PST[BLACK_PAWN][70-10*rank+file] =
                    get_data(rank*8+file-8) + PAWN_OFFSET;
            }

            for (int piece = KNIGHT; piece <= QUEEN; piece++) {
                PST[BLACK | piece][70-10*rank+file] = -(
                    PST[WHITE | piece][10*rank+file] = get_data(
                        (rank & 4 ? rank ^ 7 : rank)*4 + (file & 4 ? file ^ 7 : file) + piece*16+32
                    ) + QUADRANTS[piece*4-8+rank/4+file/4*2]
                );
            }
        }
    }
    
    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST[i][j] = rng();
        }
    }
#else
    fread(&ZOBRIST, sizeof(ZOBRIST), 1, fopen("/dev/urandom", "r"));
#endif
}
