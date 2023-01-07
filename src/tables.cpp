int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-3, -13, 13, 19, 16, 13, -7, -1};
int16_t DOUBLED_EG[8] = {29, 17, 11, 2, 5, 11, 18, 33};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 21
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 5
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 11
#define ISOLATED_PAWN_EG 14

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
    unpack_full(0, PAWN, "        B]aDAB9+I`WPLK=4FR]\\YO=4J[jhdYK<Ve}ythSBE f~sm1e        ", 1.04, 9); // average: 48
    unpack_full(1, PAWN, "        ''0731*0$ +,*&'''((&$(,+.-+(*.238D8<<=>>]Fm~yvtw        ", 1.235, 94); // average: 117
    unpack_full(0, PASSED_PAWN, "        3>520..623'& #/442'$'*:<281146FB;/:5=GOVOMBCWZ~T        ", 1.297, -19); // average: 5
    unpack_full(1, PASSED_PAWN, "        #$ \"$%-$&,'&&(+'5:40,-23DF>6149?UTM;6;FMd~Y817EE        ", 2.045, -8); // average: 28
    unpack_full(0, KING, "=>9084EI<7/%(2;A*-  \"$.+'-)+'*,\",624-11':50953.2G7::7/$8~_mb[LUT", 2.308, -53); // average: 0
    unpack_full(1, KING, "5:<71:7+>FLRQLC;HPY[[VNHOX^aa]VOWcgfhfbU^stnots_S{xrrv~S ZY]^[]'", 1.692, -88); // average: 4
    unpack_half(0, QUEEN, " )),,3523440211/?066?JAG@.E71G_b", 1.0, 636); // average: 660
    unpack_half(1, QUEEN, "9% !3,+2:CJGKW]fNjqwWevvVtp~daUS", 1.359, 1259); // average: 1327
    unpack_half(0, ROOK, "/6:C /56*60237;?DJSZRfjw`]x~uv||", 1.075, 289); // average: 338
    unpack_half(1, ROOK, " ++)('),,/559=><AEB>E??9CJBC>CAA", 1.0, 628); // average: 653
    unpack_half(0, BISHOP, "IHFGUZWRS^[Y[Y]fY\\jp`qv~SZa\\7/ 3", 1.111, 193); // average: 254
    unpack_half(1, BISHOP, " 1(/38<=:DJM6FPR9NLT<HMF:KILPRVQ", 1.0, 362); // average: 397
    unpack_half(0, KNIGHT, "ILOQNPWTPYY\\Y^__a`ig`tx~ZZqt 4+d", 1.842, 146); // average: 251
    unpack_half(1, KNIGHT, "'8KOFVYaNahp\\ivx^rw~YeonM_ah et`", 1.228, 275); // average: 350
    
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
