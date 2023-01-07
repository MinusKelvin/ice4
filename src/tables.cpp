int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-4, -11, 10, 15, 13, 12, -6, -1};
int16_t DOUBLED_EG[8] = {29, 16, 13, 5, 7, 12, 17, 29};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 18
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 7
#define TEMPO_EG 4
#define ISOLATED_PAWN_MG 10
#define ISOLATED_PAWN_EG 13

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
    unpack_full(0, PAWN, "        5NS866- <QKD@?1(9DONLB1(>N[ZWM>0ITijeYE6@\"enf[8^        ", 1.0, 15); // average: 44
    unpack_full(1, PAWN, "        %%-40.(-# *+)%'$%''&&(**+.-)*-017D;<>=?:gKg{p~xt        ", 1.259, 91); // average: 114
    unpack_full(0, PASSED_PAWN, "        4B520./:46&$ $.953%\"(+:?29.258GD:-92>IRZIG-?RYoS        ", 1.0, -17); // average: 2
    unpack_full(1, PASSED_PAWN, "        $$ \"$&-$',(&&','7<51,-45HI@836=CZWO?7>IRa~a<95GJ        ", 1.894, -8); // average: 28
    unpack_full(0, KING, "?C>5<9JN@<4+,7@F-1$$%&2-)7/.*,- .;>8535&D>DB?<8.ZKQG@<48q|~aYWtT", 2.109, -61); // average: 2
    unpack_full(1, KING, ":;>82;8+>GOUTNE<JR[_`[RJOXadeaYSXdhklkgZ\\tssrwvbLxsruy~\\ MR\\`_X&", 1.539, -82); // average: 4
    unpack_half(0, QUEEN, " )+-,452332.1//->/31>F=B>/A2-;ZY", 1.0, 540); // average: 562
    unpack_half(1, QUEEN, "9%  4)-29BKHHU]fLgnvQbtvSnm~\\bRQ", 1.384, 1203); // average: 1270
    unpack_half(0, ROOK, "-58A .23*6./147:?HMSL_eq]Xswpry}", 1.0, 239); // average: 281
    unpack_half(1, ROOK, " ,*)'&(,-.448==:@B@<D>>7CIAC=BA?", 1.0, 616); // average: 640
    unpack_half(0, BISHOP, "KLNNY`]XYb`^`]ai^blrcsv~V_c]<2 )", 1.122, 146); // average: 211
    unpack_half(1, BISHOP, " 94;AEIHGPWZBR\\^DYW_HRYRBVTVT[]]", 1.0, 343); // average: 389
    unpack_half(0, KNIGHT, "HKOQMPVTOXY\\Y\\^^`^ig^uy~WXpr!0 Y", 1.625, 118); // average: 209
    unpack_half(1, KNIGHT, "7CSTLX_eUekr_jxyasy~[hooRaci `sc", 1.347, 256); // average: 344
    
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
