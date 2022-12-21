int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

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
            white_section[rank+file] = white_section[7+rank-file] = v;
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
    unpack_full(0, PAWN, "        8QLGH^cC?RQNRSeF>OTZ\\YZDCZXadeaHD`ctuzkQ~Og|y^5>        ", 1.229, -5); // average: 47
    unpack_half(0, KNIGHT, "CFHKGHPNJSTWUZ[Z]\\gd]rw~WUpt .&a", 1.762, 149); // average: 242
    unpack_half(0, BISHOP, "HFECRXTOS[YWZW[dX\\jp_rv~U\\_[A1 0", 1.173, 200); // average: 262
    unpack_half(0, ROOK, "/7:B -13+6/1268<BIOVOdgt_[t|xx|~", 1.209, 264); // average: 317
    unpack_half(0, QUEEN, " *(,/451453/320.B397DNEIG1E:=Pkh", 1.0, 663); // average: 690
    unpack_full(0, KING, "CID6=7LNE>3++6?F/1\"!!$1.),*+'),!.529341%571;95-/D/8@90 4~ekcWSV^", 2.357, -63); // average: -4
    unpack_full(0, KING+1, "        5-7:<:C62* #+).5:2(**(+6DB;=:568fTOGAA6KD~aZQOV`        ", 1.038, -24); // average: 4
    unpack_full(1, PAWN, "        TWWYYXXQPUQSTTTOSWRPQSWPW[VSTTXT\\b`^_ZcZ}~}}}t[h        ", 2.692, -4); // average: 117
    unpack_half(1, KNIGHT, ")7KNFXY`O`hp]jwx`sx~[gqqP`dk dsa", 1.505, 330); // average: 424
    unpack_half(1, BISHOP, " 2#/7:?><GNQ8IUX:SQ[=JQK6NMOIRWT", 1.0, 444); // average: 481
    unpack_half(1, ROOK, " ./.++02/3;;>DFDJOLHQIJCPUOOCIII", 1.0, 745); // average: 777
    unpack_half(1, QUEEN, "9& !5--4;CJHMV]fQjmuWfvvUss~_`UW", 1.652, 1450); // average: 1533
    unpack_full(1, KING, ",01*%-+ 4;ADD?70=EMPPKC=DNSWVTNFMZ]\\^]\\PXjmfgmn[Qxsjlr~W*VYZ[]_0", 2.034, -87); // average: 8
    unpack_full(1, KING+1, "        !%!\"! !\"%'(%%&(%331-/053C=:55:A@UNE:9HKPOSJ;<U~g        ", 2.456, -21); // average: 28

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
