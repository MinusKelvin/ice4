int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-6, -1, -20, -24, -21, -19, -7, -8};
int16_t DOUBLED_EG[8] = {-60, -48, -34, -22, -24, -35, -48, -64};
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
    unpack_full(0, PAWN, "        9aZ;899 ?bNFBA<(;SSRNE;'>[a^YPI/Hdvrm_Q4@#^~qa3]        ", 1.087, 21); // average: 57
    unpack_half(0, KNIGHT, "FILNJLTQMVWZW]]]_^if_sy~YXqu 2(a", 1.857, 144); // average: 247
    unpack_half(0, BISHOP, "KIHGU[XRU_\\Z\\Z^fZ^kq`sv~U]b\\@3 1", 1.232, 180); // average: 249
    unpack_half(0, ROOK, "/69A .34+6/136:<CIPVPchs`\\v|xw}~", 1.207, 274); // average: 327
    unpack_half(0, QUEEN, " )(--35245403220C398DOFJF4J9;Plh", 1, 682); // average: 709
    unpack_full(0, KING, ";<8/73CG:5.&)2:?*-  !$.+&+(+'(* ,31622/$763995-/G0:?<2\"5~bg`WQT[", 2.753, -62); // average: 0
    unpack_full(0, PASSED_PAWN, "        0?641/*3.2'% \"*0/-&#'+69063469EA=1<8@KRZDLA=Tb~R        ", 1.289, -24); // average: -0
    unpack_full(1, PAWN, "        $/251/0. '+*'%-$$/'# '2(,7-'(-82:P<><?H>aRr}{~w{        ", 1.292, 128); // average: 153
    unpack_half(1, KNIGHT, "'5JMEWY`M`gp\\ivx_sx~YgqpN_cj esa", 1.464, 338); // average: 428
    unpack_half(1, BISHOP, " 2$/7<A@<IQT8KW[:US]>MTM9RPQOU[W", 1, 442); // average: 481
    unpack_half(1, ROOK, " ...*)-1/3::>DECJOLGPIJCOUNODKJJ", 1, 752); // average: 784
    unpack_half(1, QUEEN, "7$  4/.3:CJHKV]eNinuVeuuUrq~_`TW", 1.647, 1460); // average: 1542
    unpack_full(1, KING, "+.0+%-+ 2;BGF@70=FOSRLC=DOUYXUMFM\\__a^\\NWloiionZMztmns~R!RUYZYX%", 1.861, -82); // average: 5
    unpack_full(1, PASSED_PAWN, "        #! \"%&+$'*'''')&6842..03FE?9458?WSO?:=ENj~`D;=NM        ", 2.503, -29); // average: 18

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
