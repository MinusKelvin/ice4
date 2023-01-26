int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {9, -7, 17, 19, 21, 17, -1, 9};
int16_t DOUBLED_EG[] = {28, 19, 12, 6, 5, 12, 18, 33};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 5};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 18
#define ROOK_SEMIOPEN_EG 14

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

void unpack_smol(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=20) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[rank+file+1] = v;
            white_section[rank+file+10] = v;
            white_section[rank+file+11] = v;
            black_section[70-rank+file] = -v;
            black_section[71-rank+file] = -v;
            black_section[60-rank+file] = -v;
            black_section[61-rank+file] = -v;
        }
    }
}

void unpack_half(
    int phase, int piece, const char *data, double scale, int qll, int qlr, int qrl, int qrr
) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale;
            white_section[rank+file] = v + qll;
            white_section[7+rank-file] = v + qrl;
            white_section[70-rank+file] = v + qlr;
            white_section[77-rank-file] = v + qrr;
            black_section[rank+file] = -v - qlr;
            black_section[7+rank-file] = -v - qrr;
            black_section[70-rank+file] = -v - qll;
            black_section[77-rank-file] = -v - qrl;
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
    unpack_full(0, PAWN, "        E[`B;>6/HWOG@A55GOZXQJ:8NXgc]THBYazwodSI? h~uk4i        ", 1.061, 15); // average: 54
    unpack_full(1, PAWN, "        &'/432,/# ,,+'&%%('%%(,)..-+-/228F;@@>?;dFm~wtpy        ", 1.332, 90); // average: 115
    unpack_full(0, PASSED_PAWN, "        .;31.,+1,/%% \",0--$!%'57-60036D=5+73<FMRSM@GX]~U        ", 1.256, -15); // average: 7
    unpack_full(1, PASSED_PAWN, "        #$ !$%-$'-'&%',(6:40,.24DF>6039?WSN<6<GOb~Y616FC        ", 2.02, -8); // average: 28
    unpack_smol(0, KING, "UAEg6 \"3MSM9yu[F", 1.0, -48); // average: -6
    unpack_smol(1, KING, ")2. @SP=_hi]`ppf", 1.0, -40); // average: 8
    unpack_half(0, QUEEN, " (*-.36355423-20", 1.0, 639, 640, 642, 656); // average: 654
    unpack_half(1, QUEEN, "9.$ ,-+3,BMK2MWc", 1.0, 1271, 1327, 1265, 1331); // average: 1297
    unpack_half(0, ROOK, "03:A )21+623('12", 1.0, 279, 311, 284, 323); // average: 295
    unpack_half(1, ROOK, " &% ++()#$'&'.,)", 1.0, 627, 653, 621, 645); // average: 634
    unpack_half(0, BISHOP, "#  !.84..<8621;D", 1.0, 236, 242, 235, 249); // average: 252
    unpack_half(1, BISHOP, " /%,*1661=DG+@FI", 1.0, 369, 378, 369, 378); // average: 390
    unpack_half(0, KNIGHT, " $*.).:5,>=E;8HG", 1.0, 221, 239, 224, 245); // average: 241
    unpack_half(1, KNIGHT, " CTXK[`iVirx_qz~", 1.23, 266, 277, 266, 278); // average: 344
    
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
