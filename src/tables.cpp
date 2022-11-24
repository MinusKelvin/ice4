int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 1, 3, 3, 5, 8, 0};
#define MAX_PHASE 76

void unpack(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = white_section[rank+7-file] = v;
            black_section[70-rank+file] = black_section[77-rank-file] = -v;
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
    // Piece-square tables
    unpack(0, PAWN, "  !\"6RNB<ULK:MQU<VV]?XcocGg~%)!(", 1.296, -7); // average: 42
    unpack(0, KNIGHT, "ADFHEFLKGPQTRWWV\\Xc`[sv~VSos .!a", 1.758, 154); // average: 243
    unpack(0, BISHOP, "HEDCRUQOQYVTYWX`[Ygm]st~TZ_[C1 /", 1.246, 196); // average: 261
    unpack(0, ROOK, ".48@ +.2(2,.0259?EKRN`co^Yt{xy~}", 1.382, 247); // average: 305
    unpack(0, QUEEN, " +*-0591682.51.*G163DS@FI,E3ASqm", 1.0, 669); // average: 696
    unpack(0, KING, "RQB@H?5+(.$$ 028*?JW4=Ob9'Kcc^x~", 1.983, -63); // average: 5
    unpack(1, PAWN, "\"  #HJIIEGDDGIDBNMJG]`YUy~vl$! !", 4.044, -8); // average: 147
    unpack(1, KNIGHT, " +FI=TU]H]enYgvw]sw~YamlK^`g#fw_", 1.507, 329); // average: 418
    unpack(1, BISHOP, " 2!0/;A?;GRU5JXZ8YQ[AKQF6OMPLT]\\", 1.0, 439); // average: 477
    unpack(1, ROOK, " 441)-3507>?@GJHKQPKNHIBJRGI;@?B", 1.0, 751); // average: 783
    unpack(1, QUEEN, ":' !4-)4:?IFIU[eLhlsXcvvSqp~[_UV", 1.615, 1441); // average: 1520
    unpack(1, KING, "\"++ 17?C>DKOGMRSQZZW]mj`Y~qe3\\XU", 2.039, -85); // average: 8

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
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
