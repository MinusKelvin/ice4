int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-1, -12, 11, 17, 15, 12, -7, 0};
int16_t DOUBLED_EG[8] = {22, 14, 10, 2, 5, 8, 15, 27};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 32
#define TEMPO_MG 5
#define TEMPO_EG 1
#define ISOLATED_PAWN_MG 10
#define ISOLATED_PAWN_EG 12

void unpack_full(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            black_section[70-rank+file] = -v;
        }
    }
}

void unpack_half(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[7+rank-file] = v;
            black_section[70-rank+file] = -v;
            black_section[77-rank-file] = -v;
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
    unpack_full(0, PAWN, "        @Z_EAA7*H^WPLJ>3FR]\\YO>4K[igcYJ=YfzupfSDC iyms0e        ", 1.0, 9); // average: 47
    unpack_full(1, PAWN, "        *'0953-4' *-,((*)''&%),-.,*'+.257B8;<<>>UJh~rixf        ", 1.052, 68); // average: 87
    unpack_full(0, PASSED_PAWN, "        4<621-/743'' $/585)%(+:=4:4136FC91>7=GOUYWFHYP~R        ", 1.21, -19); // average: 5
    unpack_full(1, PASSED_PAWN, "        #& !#%.#&0*'')-(6=63./34FJ@936;@XVN<7<HNg~X/.9:J        ", 1.387, -2); // average: 23
    unpack_full(0, KING, "EE>4=7KQC<2$'4@G.0 !#&0/,1+,&./$0=63(37+A72>4539HA?98/%;rP~okIX2", 1.728, -48); // average: -4
    unpack_full(1, KING, "EIMHCLH;OW]ec]TLX`ikke^X]fmnojc\\dnrrtpl_e}{tvzxeX~zusy}T bXX[U\\+", 1.417, -92); // average: -0
    unpack_half(0, QUEEN, " ),-+3632431202/=/56<G?E:*B6%>W_", 1.0, 562); // average: 584
    unpack_half(1, QUEEN, "=) %6,+4<ELJMY`iOmtyYcwuYxp~kbUM", 1.122, 994); // average: 1052
    unpack_half(0, ROOK, "07;D 187*61427<@CJT[Rejw^\\v}prxy", 1.0, 299); // average: 345
    unpack_half(1, ROOK, " )(%&#$'(+//25647973832-6=555878", 1.0, 472); // average: 488
    unpack_half(0, BISHOP, "IHHIU[YSS^\\[\\[_gY^kqaqv~RZa\\/* 1", 1.045, 196); // average: 253
    unpack_half(1, BISHOP, " .)-03786>BE4@HH7FDJ9AE>8D@FQNPJ", 1.0, 264); // average: 294
    unpack_half(0, KNIGHT, "KNRTQSYWR[[_[`bbabki`tx~Z[ps 4+b", 1.794, 146); // average: 251
    unpack_half(1, KNIGHT, "(8JNGSYaM_gnZftv[ot{V`ihI[\\c br\\", 1.0, 194); // average: 253
    
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
