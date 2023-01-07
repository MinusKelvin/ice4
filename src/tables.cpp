int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-8, -15, 12, 16, 14, 13, -8, -4};
int16_t DOUBLED_EG[8] = {41, 24, 18, 9, 10, 18, 26, 41};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 54
#define TEMPO_MG 10
#define TEMPO_EG 9
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
    unpack_full(0, PAWN, "        :VZ:78. @XPGCB2(<ITRPE2(ASb`]RA1LZutoaJ8M'q~yh=h        ", 1.092, 17); // average: 53
    unpack_full(1, PAWN, "        $%.50-)-\" **)%'$%('&%'+),0.*+.12:H=>@?B<dNj{q}~x        ", 1.498, 120); // average: 148
    unpack_full(0, PASSED_PAWN, "        8J:8301=7:&% %1:64$\"),>C3:.47;MJ?.<3AOYeCH)<O^~V        ", 1.024, -19); // average: 2
    unpack_full(1, PASSED_PAWN, "        $$ !%'.%'+'&&',(7;51,-45HH@836<CYVP@8@JSh~cD?>JP        ", 2.601, -15); // average: 36
    unpack_full(0, KING, ">A<4:8HL?;3+,6>D.0%#$&2.)4-,(*+ /8;9321%B?EDD=7-\\NRPG@98}{~]^Uoa", 2.882, -74); // average: 11
    unpack_full(1, KING, "6692-53'9CJPOI@8DMW[\\VMDLT]aa^VOUbfhjheW[qqqputbLwqorw~Z KOX\\]V%", 1.944, -98); // average: 4
    unpack_half(0, QUEEN, " +-0099698737431H5:8KSHKJ7K8:Lml", 1.0, 712); // average: 742
    unpack_half(1, QUEEN, "7%! 4,/49BKHHU]eLflvPctuRmp~X_RR", 1.846, 1499); // average: 1588
    unpack_half(0, ROOK, ",57? .12+6/0257:?HLQL^cn]Wquqt{~", 1.311, 277); // average: 332
    unpack_half(1, ROOK, " /0.+).103;;@EGDJNLGPIIAPVNPELKH", 1.0, 789); // average: 821
    unpack_half(0, BISHOP, "LLMNY`]WZc`]a\\bj_bmrctw~X`d^A3 *", 1.385, 169); // average: 250
    unpack_half(1, BISHOP, " >6@IOSQO\\dhH]jnKgeqP_g_Fd`bYgkh", 1.0, 445); // average: 501
    unpack_half(0, KNIGHT, "EHMNJMTQMVWZX[]]_^if^ty~WWoq .\"Z", 1.932, 142); // average: 247
    unpack_half(1, KNIGHT, "6?QSLX^dSdjr^iwyasx~ZhqpP`cj `rb", 1.712, 336); // average: 446
    
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
