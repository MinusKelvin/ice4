int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

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
    unpack(0, PAWN, "        9PKEG[_@?QPLQQbD>OSXZWWBDYW_ab^DE`bqqufM~Pgzu[4;        ", 1.276, -5); // average: 48
    unpack(0, KNIGHT, "FGKKLHHDEGNNORKJJQSWVTTLRWWXZZZVXXac`f]]Yikr~zp]QTfrktU[ .8Pk\"87", 2.068, 134); // average: 243
    unpack(0, BISHOP, "PPMMIKJOZ[XSVWZSW_\\\\Y\\]W\\Z_dd][\\X[golj`]Ujhxx~sfWY^[`ed[C:A1H AP", 1.55, 174); // average: 263
    unpack(0, ROOK, "-4<@C:;4\"*03431 )0-.31;..17:>:<8<CJSWTQHHZ\\myspXYWn|wy_hrsy}{}z~", 1.245, 264); // average: 319
    unpack(0, QUEEN, "\")+-*%, .15138:1041105;92/0-345:9/45<>9D8C:IL\\VOB*;:;TCM3I`el~WI", 1.016, 669); // average: 697
    unpack(0, KING, "CID6;6LOF>4++5>E02#\"!$0-*-++'(+ /528240%671;96-.C.8?9/ 4~eibWSV[", 2.376, -63); // average: -4
    unpack(1, PAWN, "        TWXYYXXRPURSTTTOSWRQQSWQW[VSTTXT\\b`^_[d[}~}|}t[i        ", 2.691, -5); // average: 117
    unpack(1, KNIGHT, ",6JOOM<*HZZ``XWFM_goofaQ]hvvwuj]^pvz~xtbZgqslng\\P`eik`_O epe]rc&", 1.566, 327); // average: 424
    unpack(1, BISHOP, ")7 6527!@?IDDA@8BPWXWQIA@O\\_]YO;>YT_bYYBFNXPSTTC;VSXRSP=UXZ\\W`VI", 1.0, 438); // average: 481
    unpack(1, ROOK, "/9:9443 79=<92,.<BFFA?52LRSNJJE@W]YSNPOL_WXPGKHP]a\\XWOXPNTRRQPOI", 1.0, 736); // average: 776
    unpack(1, QUEEN, "@2*'*! 5;=5:6-%8BEPJLKE<GX[ja_XUNbkruoq[ScosvwpcQpsz~puc^`XYXSee", 1.811, 1433); // average: 1528
    unpack(1, KING, "-01+&.+ 4;ADD?80=EMPPLD=DNTWWTNFM[^]_]\\PXjnfgmn\\Qxsklr~W+VYZ\\]_0", 2.038, -89); // average: 7
    unpack(0, KING+1, "        5-79:7?31) \")&+291')'&)3C@9;9357dSMF?@5IC~aWPKR^        ", 1.034, -23); // average: 4
    unpack(1, KING+1, "         $ !! !\"%&'%%&(%330,/053C=:44:@@UNE:9HJOPSJ<<U~f        ", 2.449, -20); // average: 29

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
