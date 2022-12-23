int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-6, -1, -20, -24, -21, -19, -7, -9};
int16_t DOUBLED_EG[8] = {-60, -48, -34, -22, -24, -35, -48, -64};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 50
#define TEMPO_MG 6
#define TEMPO_EG 3

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
    unpack_full(0, PAWN, "        9aZ;789 ?bNFBA<(;SSQNE;'?[a^YPI/Idvrm_Q5B%^~pa4\\        ", 1.101, 21); // average: 57
    unpack_half(0, KNIGHT, "FHKMJLSPLVVZW]]]_^hf_sy~YXqu 2(a", 1.865, 145); // average: 248
    unpack_half(0, BISHOP, "KIHGU[XRU_\\Z\\Z^fZ^kq`rv~U^c\\@4 2", 1.239, 181); // average: 250
    unpack_half(0, ROOK, ".69A .34+70136:=CIQVQchs`]w|xx}~", 1.214, 274); // average: 328
    unpack_half(0, QUEEN, " )(-.46356514331C4:9EQHLG5K;;Qmi", 1.0, 683); // average: 711
    unpack_full(0, KING, ";<7/73CG:5.&)2:?*-  !$.+&*(+'(*!,20612/$663995-/G0:?<2\"5~bg_WQT[", 2.763, -60); // average: 2
    unpack_full(0, PASSED_PAWN, "        1?751/*3.2'% \"*1/-'#'+69063469EB=1<8@KRZCLA=Tb~S        ", 1.29, -25); // average: -0
    unpack_full(1, PAWN, "        $/261/1. '+*'$-$$/'\" '2(,7-'(,829O<><?H>_Qq}{~v}        ", 1.294, 128); // average: 153
    unpack_half(1, KNIGHT, "'6JNEWY`M`gp\\iwx_sx~YgqpN_cj esa", 1.467, 337); // average: 428
    unpack_half(1, BISHOP, " 2$07;@?<IQT8KWZ:TR]>MTL8QPQOU[V", 1.0, 442); // average: 481
    unpack_half(1, ROOK, " ..-*)-1/3::>DECJOLGPIJBOUNNEKJI", 1.0, 752); // average: 784
    unpack_half(1, QUEEN, "7$! 3..3:CJHKV]fOjnvVfuuUqq~_`TW", 1.637, 1461); // average: 1542
    unpack_full(1, KING, "+/0+%-+ 3;BGF@70=FOSRLD=EOVYXUMFM\\__a^\\NWloiionZMztmns~R!RUYZYX%", 1.869, -82); // average: 6
    unpack_full(1, PASSED_PAWN, "        #! \"%&*$'*'''')&6842..03FE?9458?WSO?:=ENk~`D;=NL        ", 2.506, -29); // average: 18

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
