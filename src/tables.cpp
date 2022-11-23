int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

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
// uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Piece-square tables
    unpack(0, PAWN, "\"$' DNKDENIGFLKKKQOOYd_^~~{v#& $", 2.491, -11); // average: 92
    unpack(0, KNIGHT, "HNPSPT[\\T_ag`gllhjtucux~Y`pr V]d", 1.661, 208); // average: 315
    unpack(0, BISHOP, "%$ \"4:735?B?6=CL9AKS>OOR.?A@#24;", 1.0, 323); // average: 349
    unpack(0, ROOK, "(6>E 06;-:;=?HKLS[^^`gjlhiqucdfg", 1.0, 472); // average: 520
    unpack(0, QUEEN, " %!&/-2-255168;=F>JNHYZ^DDXZBSZX", 1.0, 1025); // average: 1056
    unpack(0, KING, "18& .,('&1243?BCHWWV\\qoh`~ulAnic", 1.254, -45); // average: 7
    unpack(1, PAWN, "\"$' DNKDENIGFLKKKQOOYd_^~~{v#& $", 2.491, -11); // average: 92
    unpack(1, KNIGHT, "HNPSPT[\\T_ag`gllhjtucux~Y`pr V]d", 1.661, 208); // average: 315
    unpack(1, BISHOP, "%$ \"4:735?B?6=CL9AKS>OOR.?A@#24;", 1.0, 323); // average: 349
    unpack(1, ROOK, "(6>E 06;-:;=?HKLS[^^`gjlhiqucdfg", 1.0, 472); // average: 520
    unpack(1, QUEEN, " %!&/-2-255168;=F>JNHYZ^DDXZBSZX", 1.0, 1025); // average: 1056
    unpack(1, KING, "18& .,('&1243?BCHWWV\\qoh`~ulAnic", 1.254, -45); // average: 7

    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 23; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST_PIECES[i][j] = rng();
        }
    }
    // ZOBRIST_CASTLE_RIGHTS[0] = rng();
    // ZOBRIST_CASTLE_RIGHTS[1] = rng();
    // ZOBRIST_CASTLE_RIGHTS[2] = rng();
    // ZOBRIST_CASTLE_RIGHTS[3] = rng();
    ZOBRIST_STM = rng();
#else
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
//     fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
