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
    unpack_full(0, PAWN, "        GkcGDEH1KlVPLLJ7G^\\[XOJ7Iddd_WV>PfrtsjcEJ Sx~qQe        ", 1.211, -2); // average: 47
    unpack_half(0, KNIGHT, "EHJLHKRPLUVYV\\\\\\^]hf^sx~XXqu 1(a", 1.817, 146); // average: 246
    unpack_half(0, BISHOP, "JHGFSYVPT]ZY[Y]eY]jq_rv~U]b\\A3 1", 1.211, 179); // average: 245
    unpack_half(0, ROOK, "/69A .34+6/1269<AHPVObgs`\\v|xx}~", 1.207, 270); // average: 323
    unpack_half(0, QUEEN, " )(--35245403320C4:9DPGKH5K;>Qlh", 1.0, 676); // average: 703
    unpack_full(0, KING, ">?:1:5GK=70'*4<B*.  \"$0,&*(+((+!*207230%461:96/0C/:@=3#6~djcZUW`", 2.557, -60); // average: 1
    unpack_full(0, KING+1, "        2,258:C31)# **5071('&(12>?564784WJC@:H8F`~]QFMP>        ", 1.181, -22); // average: 2
    unpack_full(1, PAWN, "        #./3/-.* ()*'$+\"#.'%\"%.&+4,(''0,7M?>6-72lb~vfP8R        ", 1.579, 121); // average: 146
    unpack_half(1, KNIGHT, "'6JNEWY`N`hp]iwx`sx~YgqpO_cj dsa", 1.462, 336); // average: 427
    unpack_half(1, BISHOP, " 2#/8<A?=IQT8KW[;US]?MTM9QOQNTZV", 1.0, 440); // average: 479
    unpack_half(1, ROOK, " -..*)-2.29:=CEBINKGPHIBOTMNCIHH", 1.0, 749); // average: 780
    unpack_half(1, QUEEN, "7$  4//4;DJHLV^fOjnvVeuvUrq~^_TW", 1.618, 1455); // average: 1535
    unpack_full(1, KING, ",/0+%-, 2:AED?70=ENQPKC<DNTWVSMFMZ]]_]\\OVjmggnn[Mvqkmr~U\"PSXZZ[*", 1.955, -85); // average: 5
    unpack_full(1, KING+1, "        !$ \"! !!&((%&()&974/3496MFA9:?FGcXS@@NSZj~\\DDU|f        ", 1.978, -20); // average: 24

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
