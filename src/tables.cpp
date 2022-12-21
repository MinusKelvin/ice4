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
    unpack_full(0, PAWN, "        $B<68SY1-DB>DE[5+@FMPMN21NKVZ[V72UYnnucByA^wsS ,        ", 1.0, 21); // average: 52
    unpack_half(0, KNIGHT, "CFHKGHPNJSTWUZ[Z]\\ge]sw~WUpt .&a", 1.754, 149); // average: 242
    unpack_half(0, BISHOP, "IFECRXTOS\\YXZX[dY\\jq_rv~V\\_\\A1 0", 1.182, 177); // average: 241
    unpack_half(0, ROOK, "/7:B -13+6/0269<BJPVOdgt`[u|yx|~", 1.201, 265); // average: 318
    unpack_half(0, QUEEN, " *(,/451453/321.B397DOEIG1E9>Pjh", 1.0, 672); // average: 699
    unpack_full(0, KING, "DID6=8LNE>4++6?F/1\"!!$1-),**'(+!.528241%581;85-/D.8@:0 5~ejcWSV^", 2.354, -65); // average: -5
    unpack_full(0, KING+1, "        5,7:<:C62* #+).5:2()*(+6DB;=:578fTPGAA6KF~aZQNV_        ", 1.033, -25); // average: -2
    unpack_full(1, PAWN, "        +014422%#-%)*** (1&#$(0#08-(**2*:FA=?6I7~}zz|i8S        ", 1.36, 122); // average: 148
    unpack_half(1, KNIGHT, "(7JNEXYaN`hp]iwx`sx~ZfqpO_cj dra", 1.477, 335); // average: 426
    unpack_half(1, BISHOP, " 2\"/7;A><IPS8JV[:TR]>LSL7POPMSXU", 1.0, 440); // average: 478
    unpack_half(1, ROOK, " ./.**/2/3::>CECINLGPHIBOUNNCIHI", 1.0, 748); // average: 779
    unpack_half(1, QUEEN, "9%  4--3;CIGLV\\eQjmuWfvvUrr~^`UV", 1.638, 1450); // average: 1532
    unpack_full(1, KING, ",00*%-+ 4;ADD?70=ELPPKC=DNSWVSMFMZ]\\^][PXjmffmn\\Qwrjlr~W*VYZ[]^0", 2.037, -87); // average: 8
    unpack_full(1, KING+1, "        !%!\"! !\"%'(%%&(%331-/053C=:55:A@UNE::HJONSK<=U~g        ", 2.464, -21); // average: 23

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
