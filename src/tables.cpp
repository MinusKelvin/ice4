int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int16_t KING_SAFETY_MG[28] = {-19, -24, -21, -17, -15, -13, -11, -7, -2, 3, 11, 18, 26, 32, 41, 44, 44, 45, 33, 23, 14, 0, 1, -36, 0, -59, -4, -31};
int16_t KING_SAFETY_EG[28] = {-21, -45, -41, -37, -31, -26, -26, -26, -27, -27, -29, -29, -30, -28, -28, -22, -16, -11, -1, 9, 19, 31, 37, 56, 57, 81, 83, 94};

void unpack(int phase, int piece, const char *data, double scale, int offset) {
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
    unpack(0, PAWN, "$&&&!#\" 3EA:<KP88FDBDFR<7DGKMKJ9<MJRSSO;AUZdd`TAtlr~u\\7N%%\"%%'#!", 1.608, -5); // average: 50
    unpack(0, KNIGHT, "GHKJKGHDFHNMNPKJJRSWVTTMRWXXZ[ZWXYac`e]]Xhkr~zq\\QTfqkuU\\ 1:Rm&:7", 2.134, 133); // average: 246
    unpack(0, BISHOP, "MMKHFFGLXYVORSWQU]ZZVZ[U[X^cb[Z[VZfnlj^\\Rihxy~sdUY]Z_feZ@9B1I AP", 1.49, 184); // average: 267
    unpack(0, ROOK, "-39;>472\"*012// *1-/219./27;=:;7<BHRTQNGHWZitnlVWUkxtw]doqx}|~z{", 1.353, 267); // average: 324
    unpack(0, QUEEN, "\")*+(\"+ .04.038113/0.6:92/0-346:9046=?:E7A;IMZVNB,;;=UEN4J`el~ZL", 1.075, 669); // average: 699
    unpack(0, KING, "@FB6;6IJC?7//7>B36(&%(3/23+# (0'9:-(\".5/B<-1--29P7<@84'>~lsk]YXY", 2.566, -70); // average: -2
    unpack(1, PAWN, " !\" \"!\" IIGHGGHFEGCCDDFCHIDBBDIEONJGGIMK_a[VVW_Yy}vlnw~y  !!!! !", 3.816, -5); // average: 140
    unpack(1, KNIGHT, ")1DJIF7&?VU\\[SQ@I\\dkjb]LYettusgY\\nuz~xsaZgrtmpg\\N`fjla`N epe]sc%", 1.523, 325); // average: 416
    unpack(1, BISHOP, "-9 7627%<AIEEA@4CQYXVRJCAR^_]ZQ=B^Wad]^FMV]SWZ\\H>[X]WXVAYX[b^cVM", 1.0, 426); // average: 472
    unpack(1, ROOK, "/9860.1 47;;80)+9?CB<;2/JPQLGGB=V]ZTQQOK^XZSKOKQ\\b^ZYQYQMRQPNLLI", 1.0, 726); // average: 764
    unpack(1, QUEEN, "B4+')  6;@7=9.%9BGRKMJD<GX]kb`XSMcmsvppYSeotvwoaPpuz~pua^aZZYTdd", 1.871, 1417); // average: 1515
    unpack(1, KING, "*142,3- /<EIIC:.9GTXXRF:@Q^ih]QCI^jrtk_MSlytwzrXKvtnqu~S'PRTWYZ/", 2.135, -103); // average: 4

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
