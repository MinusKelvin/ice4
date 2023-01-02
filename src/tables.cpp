int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-6, -14, 13, 19, 16, 13, -8, -3};
int16_t DOUBLED_EG[8] = {36, 21, 13, 4, 6, 15, 21, 39};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 49
#define TEMPO_MG 6
#define TEMPO_EG 3
#define ISOLATED_PAWN_MG 12
#define ISOLATED_PAWN_EG 15

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
    unpack_full(0, PAWN, "        @Z]A>>6)F\\RLHG:1CNXWTK90FVdb^UF8P_xupdO=D `~qf0a        ", 1.157, 10); // average: 50
    unpack_full(1, PAWN, "        &'1620*/# ++)&&&&((&$(,)./-)+.23:F:>>?@?aJq~~}t}        ", 1.366, 113); // average: 139
    unpack_full(0, PASSED_PAWN, "        4A631/.724&% \"/420%#(*:<160157GB;-84=HQYHH><T^~T        ", 1.308, -20); // average: 5
    unpack_full(1, PASSED_PAWN, "        #$ \"%&-$&+'&&(+'5:30,-23DF=6149?UTN<6<FMf~\\?59KH        ", 2.508, -14); // average: 32
    unpack_full(0, KING, ";<7/73CG:5.&)29?)-  !$.+&+(*'(* +204/1.$651763-/G2:=91#6~bh^VOU[", 2.72, -61); // average: -1
    unpack_full(1, KING, "+/1,&., 3<CHFA81>GOSRMD>FPVZYUNGO]a`b`]OYnpjjpo[Nzuoot~R TV[\\ZZ$", 1.837, -82); // average: 6
    unpack_half(0, QUEEN, " ))--46355413220B388CNEJE3I98Nig", 1.0, 686); // average: 713
    unpack_half(1, QUEEN, "7$  2--3:CIGKV]fNiovVevvUrp~``UV", 1.589, 1436); // average: 1515
    unpack_half(0, ROOK, ".69B .34*6/136:=CIQXQeiua\\w}wx}~", 1.186, 282); // average: 335
    unpack_half(1, ROOK, " ..,)),0.299>CDBHMJFNGGAMSLLDJHH", 1.0, 736); // average: 766
    unpack_half(0, BISHOP, "KIHGU[XRU_\\Z\\Z^fZ]jq`rv~U\\b\\>2 3", 1.227, 186); // average: 254
    unpack_half(1, BISHOP, " 2&06;@@<HPS8JVY;TR\\?MSL:QOQQV[W", 1.0, 430); // average: 469
    unpack_half(0, KNIGHT, "GJLNLNTRMWWZX]]]_^hf_sy~YXpu 3)b", 1.891, 146); // average: 252
    unpack_half(1, KNIGHT, "(6KNEWY`N`hp\\ivx_rx~YfppN_cj esa", 1.436, 328); // average: 417
    
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
