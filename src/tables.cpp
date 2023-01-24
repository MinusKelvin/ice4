int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {10, -6, 17, 21, 22, 18, 0, 10};
int16_t DOUBLED_EG[] = {26, 18, 11, 4, 4, 11, 17, 31};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 5};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 43
#define TEMPO_MG 6
#define TEMPO_EG 1
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 35
#define ROOK_OPEN_EG 12
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

void unpack_half(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[7+rank-file] = v;
            white_section[70-rank+file] = v;
            white_section[77-rank-file] = v;
            black_section[rank+file] = -v;
            black_section[7+rank-file] = -v;
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
    unpack_full(0, PAWN, "        F\\aC=?93IYQHCC88IQ[YSK<;OXgd]TICZbzwpcRJB c~rf2f        ", 1.09, 14); // average: 54
    unpack_full(1, PAWN, "        %)2732-.! *+*'&$$'%%$'+'+-*'+.116D8=>=?;]Fl~yvqv        ", 1.366, 91); // average: 116
    unpack_full(0, PASSED_PAWN, "        .<52--+0-2'' #,///&#&(56.61/25B<6,94<ELPSNHL]_~U        ", 1.33, -16); // average: 8
    unpack_full(1, PASSED_PAWN, "        $$ \"&&-%(-('&(,)7:41-/35EF>725:AWSM;7=GPd~W618HG        ", 2.12, -11); // average: 28
    unpack_full(0, KING, ":<8/62BF:6/%(2:?(+!!#%-*%*(*&)* *2-/)./'81+31.+2E57:7-&6~dng`S[X", 2.63, -59); // average: -1
    unpack_full(1, KING, "7>@;7><-BKQWVQH>MU]`_ZSLS\\add`ZT[fihkheY`uvoputaT{xrrw~U XWZ[YZ(", 1.744, -97); // average: 3
    unpack_half(0, QUEEN, " '),/26265526253", 1.0, 640); // average: 657
    unpack_half(1, QUEEN, "K9& G;25COUQK^fr", 1.0, 1290); // average: 1329
    unpack_half(0, ROOK, "/17< *0/,63627=>", 1.0, 281); // average: 299
    unpack_half(1, ROOK, "!&$ 52-,.--).0/+", 1.0, 636); // average: 647
    unpack_half(0, BISHOP, "$!! /84.0>9769<H", 1.0, 236); // average: 254
    unpack_half(1, BISHOP, " 0#+-3875@FH1CIL", 1.0, 370); // average: 394
    unpack_half(0, KNIGHT, " $*.*.:5,?>F@INO", 1.0, 224); // average: 247
    unpack_half(1, KNIGHT, " @QUO^bhViqxbr|~", 1.246, 267); // average: 346
    
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
