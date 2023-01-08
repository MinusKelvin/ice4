int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-3, -13, 13, 18, 15, 12, -8, -2};
int16_t DOUBLED_EG[8] = {29, 17, 10, 3, 5, 10, 16, 32};
int16_t VIRTUAL_QUEEN_MOB_MG[] = {-6, 37, 38, 25, 26, 18, 18, 13, 14, 7, 5, 6, 4, 2, -2, -6, -3, -17, -19, -24, -28, -31, -15, -4, -5, -5, -33, -55};
int16_t VIRTUAL_QUEEN_MOB_EG[] = {-5, 69, 25, 26, 25, 21, 23, 27, 26, 24, 20, 18, 18, 16, 19, 15, 10, 12, 7, 2, -4, -8, -19, -30, -39, -52, -58, -70};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
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
    unpack_full(0, PAWN, "        =VW;9>5&DZRIHE90AMYWVL80FWgeaVG8Sb{wqfO>C g~pk/c        ", 1.019, 14); // average: 50
    unpack_full(1, PAWN, "        )%.543-3& ),****)((''+0.0/-*-156:E:?A?A@`Lp~|wvx        ", 1.294, 89); // average: 115
    unpack_full(0, PASSED_PAWN, "        3;211,-612$% \"/332'$&):<182235EA9/;6=EOVNMBCWX~T        ", 1.251, -18); // average: 6
    unpack_full(1, PASSED_PAWN, "        $% !#%-$','%&'+'6:3/,.34FG>725;@WUN<8=HNf~Y929EF        ", 2.006, -9); // average: 27
    unpack_full(0, KING, ">@<4<6GJ97/&)2;>)- !\"$.*)/**%*.\"/934,24)=8194215J8<=8/#8~cujdPXP", 2.166, -55); // average: -2
    unpack_full(1, KING, "7>CB;B=.@MT[ZTJ>IVdhhaTIN]juuh[OUgr{}qfSYt~yz}t[Nzxttx}O UUZZWW(", 1.863, -110); // average: 1
    unpack_half(0, QUEEN, " )*,,3623441312/?166?JAG@.E71G]`", 1.0, 637); // average: 661
    unpack_half(1, QUEEN, "9% !4-,2:DKHKW^hNkqyWevvVtq~daVS", 1.351, 1260); // average: 1328
    unpack_half(0, ROOK, "/6:B /56*60236;>CJSYRejv`\\x~uv{|", 1.069, 291); // average: 339
    unpack_half(1, ROOK, " ,+*((*.,0669=>=AEC?F??:DJCC>BAA", 1.0, 627); // average: 652
    unpack_half(0, BISHOP, "JHGHU\\XRT_\\Z\\Z^fY]kq_rv~S[a\\8/ 3", 1.124, 193); // average: 255
    unpack_half(1, BISHOP, " 1(048==:DKM7GQS9NMU<HMG:KILPRVQ", 1.0, 362); // average: 397
    unpack_half(0, KNIGHT, "JLPRORXUPYZ]Z_``a`jh`ty~ZZqt 5+d", 1.854, 145); // average: 252
    unpack_half(1, KNIGHT, "'8KNFVYaNahp\\hvx^rw~XdnnL^`g ds^", 1.236, 274); // average: 349
    
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
