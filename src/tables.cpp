int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-6, 1, -20, -22, -20, -15, -3, -6};
int16_t DOUBLED_EG[8] = {-60, -47, -33, -23, -22, -35, -48, -63};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define KING_OPEN_FILE_MG 19
#define KING_OPEN_FILE_EG 0
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 49

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
    unpack_full(0, PAWN, "        ?b]<?@@(DbQGHGC0@UWRTKB/D\\d_^UO6MdxrqdW;> ^~td5\\        ", 1.145, 11); // average: 52
    unpack_half(0, KNIGHT, "FILNKMTQMVWZW]]]_^if_sy~YXqu 2(a", 1.867, 145); // average: 248
    unpack_half(0, BISHOP, "LJIHU\\XRV`\\[\\Z_fZ^kq_rv~U^b\\@3 2", 1.239, 180); // average: 249
    unpack_half(0, ROOK, "/69A .34+6/135:<BHPVPchs`]v|xx}~", 1.201, 275); // average: 328
    unpack_half(0, QUEEN, " )(--3524550322/C398COFKF4J::Qkg", 1.0, 684); // average: 711
    unpack_full(0, KING, ":;7/73CF:5/')2:?)-! \"$.+&+),((* +31622.$663994-.G0:?<2\"4~bg`XQT\\", 2.734, -58); // average: 4
    unpack_full(0, PASSED_PAWN, "        /<450-(1,/$' !(/-+%%&)47.41557C?90:8>IOWGNA=Sa~T        ", 1.318, -23); // average: 1
    unpack_full(1, PAWN, "        $/271/0. '+,'%-$$/'$!'2(,6,((-829O;?<?H>_Sq}{~w}        ", 1.299, 127); // average: 153
    unpack_half(1, KNIGHT, "'5JMEWY`M`gp\\ivw_rx~YgppN_cj dsa", 1.466, 338); // average: 428
    unpack_half(1, BISHOP, " 2$07<A@<IQT8KWZ:US]?NTM9RPQPV[W", 1.0, 442); // average: 481
    unpack_half(1, ROOK, " ...*)-1/3:;>DECJOLGPJJCOUNOEKJJ", 1.0, 752); // average: 783
    unpack_half(1, QUEEN, "7$! 4//3;DJHKV]gNjnvVfuuUqq~``UW", 1.649, 1460); // average: 1542
    unpack_full(1, KING, "+/0,%-, 3;BGF@81=FOSRLD=EOVYXUNFM\\__a^\\OWloiionZNztmns~R!RUYZYX%", 1.872, -82); // average: 6
    unpack_full(1, PASSED_PAWN, "        #\" !%&+$'*'%&')&6941..03FE?8458?WSP>9=ENk~aD;=NL        ", 2.494, -28); // average: 18

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
