int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-4, -13, 11, 16, 14, 11, -7, -2};
int16_t DOUBLED_EG[8] = {29, 17, 10, 2, 4, 11, 17, 32};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 9
#define ISOLATED_PAWN_EG 11
#define CONNECTED_PAWN_MG 8
#define CONNECTED_PAWN_EG 7

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
    unpack_full(0, PAWN, "        A]`B?@7)CYOHDC6.CQYXUK:1IZfd`UH:Ud|xsfR@C e~ql1d        ", 1.024, 10); // average: 47
    unpack_full(1, PAWN, "        ))1731,1$ ,-+'''')(&$(,+/.,)+.24:E9===@>_Hn~zvtv        ", 1.266, 90); // average: 114
    unpack_full(0, PASSED_PAWN, "        1=41/,,413&& \"/32/%\"&)8:/60/25D@8+83;FNUOLABWY~S        ", 1.241, -16); // average: 7
    unpack_full(1, PASSED_PAWN, "        #$ !$&-$&-(&&(,'5:41,.24DF>6149?UTM;6;FMc~Y807EE        ", 2.027, -8); // average: 28
    unpack_full(0, KING, "=>9083EI<7/%(2;A*-  \"$.+'-)+'*,!,624-21':60953.2G7::7/$8~_mb[LUT", 2.307, -53); // average: 1
    unpack_full(1, KING, "5:<72:7+>FLRQLC;IPY\\[VNHOX^aa]VOWcgfhfcU^stnots_S{xrrv~S ZY]][\\'", 1.698, -89); // average: 3
    unpack_half(0, QUEEN, " )*-,3633441212/@177@KBH@/F71G`b", 1.0, 637); // average: 661
    unpack_half(1, QUEEN, "9% !3,+2:CJGKW^gNkqwWevvVsp~daUS", 1.355, 1262); // average: 1329
    unpack_half(0, ROOK, "/6:C /56+70337<?CKSZRfkw`\\x~uv|}", 1.071, 290); // average: 339
    unpack_half(1, ROOK, " ++)('),,0559>><AEB>E??9CJBC>CAA", 1.0, 629); // average: 653
    unpack_half(0, BISHOP, "IHGGTZWRS^[YZY]eY\\jp`qv~S[a\\7/ 3", 1.123, 193); // average: 254
    unpack_half(1, BISHOP, " 1(/37<=9DJM6FPR9NLT<HMF9KILPRVQ", 1.0, 363); // average: 398
    unpack_half(0, KNIGHT, "ILOQNPWTPYY\\Y^__``ih`ty~YZpt 4+c", 1.851, 146); // average: 252
    unpack_half(1, KNIGHT, "'8KOFVYaNahp\\ivx^rw~YennM^ah et`", 1.233, 275); // average: 351
    
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
