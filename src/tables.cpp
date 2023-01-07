int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-18, -13, 1, 6, 0, 4, -10, 2};
int16_t DOUBLED_EG[8] = {18, 13, 26, 16, 17, 27, 20, 38};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 73
#define TEMPO_MG 22
#define TEMPO_EG 24
#define ISOLATED_PAWN_MG 17
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
    unpack_full(0, PAWN, "        IZO@15. NUDDA<0\"JHHKK=2%TTYWLE:,fnzaY]>81Jd~y`QT        ", 1.637, 12); // average: 63
    unpack_full(1, PAWN, "         #3B5626\"%355..0&+-,+/0100-(.48=DP9BWCRLl{trn~~}        ", 1.356, 96); // average: 128
    unpack_full(0, PASSED_PAWN, "        SlXPJR\\gZZMEDHUmBUK@NNct?QWYhtpw 9R_jk~olefUVv~Q        ", 1.051, -59); // average: -13
    unpack_full(1, PASSED_PAWN, "        .($ !'.*.0))#+1+EC:6/07<YVJ>77EM{ogI9Tak~gfILJXb        ", 1.87, -16); // average: 32
    unpack_full(0, KING, "`bX@SIad^VOGFMZ^GJ8579HD:9,\"!.0%782 \"-1*.O<6=ZOH;J:P@DYOWXW5>Kd~", 3.258, -158); // average: -40
    unpack_full(1, KING, "6@JQCOB3HT[`a\\SHP]iooh]UVerzzria\\lv}~yrdcltz{usaZiojsvl\\,EMcZ[P ", 2.09, -125); // average: 7
    unpack_half(0, QUEEN, ".+-:@DE@AB<6<C9APEMEkl]WY@R8 F\\j", 1.0, 645); // average: 683
    unpack_half(1, QUEEN, "+./- (2<6DSTJTajJ`fu?IkoCXh~MESQ", 1.502, 1197); // average: 1264
    unpack_half(0, ROOK, ")24@ 797,A23-53AGKSX_ooqng~{ndvv", 1.029, 318); // average: 365
    unpack_half(1, ROOK, " .02)&*,*#-389===>A>:<<;>HHG@FIE", 1.0, 648); // average: 672
    unpack_half(0, BISHOP, "N^JI^bgU\\b^aV]arXbw~nt}~Xkji9=2 ", 1.302, 195); // average: 275
    unpack_half(1, BISHOP, " 2'50:6E:ESV:OXYESR[CFPE0DB?;ECL", 1.0, 347); // average: 382
    unpack_half(0, KNIGHT, ":FMJMLSTKYZ^T`acbbroboz~\\^tl 0BV", 2.217, 133); // average: 259
    unpack_half(1, KNIGHT, "L?QWUXcdWbkv_kz}`qz~VcmnL][^ MTT", 1.438, 270); // average: 360
    
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
