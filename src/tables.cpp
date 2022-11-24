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
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Piece-square tables
    unpack(0, PAWN, " %*)8NK@<PHG;JLO=QQUCX`heSl~\")&!", 1.538, -8); // average: 50
    unpack(0, KNIGHT, "BEGIFGNMIRSVTYZY][fc\\sw~WTps .%`", 1.73, 152); // average: 242
    unpack(0, BISHOP, "FDBAQUQNQZXUXWYbZZho]tu~SZ_Z@1 .", 1.149, 203); // average: 263
    unpack(0, ROOK, "/7;D ,04*6/2379=CIOVPdfs`[u{xx}~", 1.24, 264); // average: 319
    unpack(0, QUEEN, " *(,048/572/631.G4:7ESDJH0G9@Qjh", 1.0, 663); // average: 691
    unpack(0, KING, "RQB@JA6,.1# #+('(2142129A'4=~hkg", 2.052, -63); // average: -7
    unpack(1, PAWN, " ## HJIIEHEEGJECNNJH]`YUz~vm\"!! ", 3.792, -5); // average: 142
    unpack(1, KNIGHT, "(5IMAWX_L_fo[hvw`sx~\\grqOael etc", 1.487, 328); // average: 420
    unpack(1, BISHOP, " /\"..:>>9ENQ4GTW8UPY>LRJ5NLNHMUV", 1.0, 439); // average: 475
    unpack(1, ROOK, " //-'*02-29:<BECIOOJOJMFNUOQCGGH", 1.0, 736); // average: 767
    unpack(1, QUEEN, "9& !2-)48?IGHU\\fKhmuUcwvSrq~[^UV", 1.639, 1437); // average: 1516
    unpack(1, KING, " *+\".5=A:BIMDMRUP\\^^]pohX~um.\\[Z", 1.995, -84); // average: 8

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
