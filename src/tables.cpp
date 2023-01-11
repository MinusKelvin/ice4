int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {3, -14, 9, 15, 12, 8, -9, 5};
int16_t DOUBLED_EG[] = {28, 19, 13, 5, 8, 14, 19, 32};
int16_t PAWN_ISLANDS_MG[] = {73, 17, 5, -6, -18};
int16_t PAWN_ISLANDS_EG[] = {-4, 2, 7, 8, 9};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 6
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
    unpack_full(0, PAWN, "        F\\^B??9/M_TNJH=8JRZZWM=8NZgfbWK@ZdzwsfTGH e~sm2j        ", 1.04, 7); // average: 47
    unpack_full(1, PAWN, "        &'1842*/\" ,-*''%%(('%)+),.,)+.127D9===><]Fm~zvsu        ", 1.231, 96); // average: 119
    unpack_full(0, PASSED_PAWN, "        0?731/.4/3(' $/101'$'+:9.50.15D?7+82:ENTMJ?@UX~Q        ", 1.242, -12); // average: 11
    unpack_full(1, PASSED_PAWN, "        $% \"$%-%'-(&&(+(6:40,-24EG>724:@VTM<6;FMc~Z928FF        ", 2.077, -12); // average: 25
    unpack_full(0, KING, "<<8073CG:6.%(19?*-  \"$.+',)*')+\"-624-21(;6195404I8::70';~^j_XKUW", 2.42, -55); // average: 1
    unpack_full(1, KING, "5:=82:8+?GMSRLD<IQY\\\\VOIOY_ba^VPWcggifcU^sunots_S|xrrv~S [Z^^[]'", 1.69, -89); // average: 4
    unpack_half(0, QUEEN, " )),,3523430201/?056?JAG?.E70G_b", 1.0, 637); // average: 660
    unpack_half(1, QUEEN, "9% !3,+2:CJHKW]fNkqwWevuVtp~daUS", 1.365, 1259); // average: 1327
    unpack_half(0, ROOK, "/6:C /56*60237;?DJSZRfkwa]x~uv||", 1.075, 289); // average: 338
    unpack_half(1, ROOK, " ++)('),,/559=><AEB>E??9CJBC>BAA", 1.0, 628); // average: 653
    unpack_half(0, BISHOP, "IHGGUZWRS^[Y[Y]eY\\jp`qv~RZa\\7. 2", 1.118, 193); // average: 254
    unpack_half(1, BISHOP, " 1(/37=<:DJM7FPR9OLU<HMF:KILPSVQ", 1.0, 362); // average: 397
    unpack_half(0, KNIGHT, "ILOQNPWTPYY\\Y^__`_ig`tx~YZpt 4+d", 1.85, 146); // average: 252
    unpack_half(1, KNIGHT, "'7KNFVYaN`hp\\ivx^rw~YennM_ah eu`", 1.227, 275); // average: 350
    
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
