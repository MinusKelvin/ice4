#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(27, 44)
int32_t DOUBLED_PAWN[] = {S(9, 16), S(-9, 15), S(10, 20), S(12, 13), S(13, 9), S(12, 21), S(-6, 17), S(13, 23)};
#define TEMPO S(13, 9)
#define ISOLATED_PAWN S(9, 7)
int32_t PROTECTED_PAWN[] = {0, S(10, 6), S(11, 5)};
#define ROOK_OPEN S(37, 8)
#define ROOK_SEMIOPEN S(18, 13)
int32_t PAWN_SHIELD[] = {S(9, -16), S(18, -34), S(21, -28), S(28, -23)};
#define KING_OPEN S(-48, 2)
#define KING_SEMIOPEN S(-13, 19)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = ":KL,&0* =E:503'$=<IGC:.)GLYTLB<3]\\v`^\\F;3(V~t^:X&+7MA<69 $488/,,$.*)+.2.04.,-989@H<J\\HJFKbTXV[nU1C1=,1/=4-/'*&.;//) +*;E4;B9>HKJ((;EIISXbZ~P[evW40- -+8/7=34.1;5JHCB98>AWTOE@<IPjkaF8Q^d~pmWTZXnkMLpC \"9R*)$^OCx*94%9PK5BXVB+E@ #$ '/241631.-..0 &57%+:?+GVT9MYb+.17 *0,&5/)%(()$%(%'&%$\" \"%#&(&&6# 3@@35A??04AN\"( *#)+0,2:<)::< W\\\\]aojats~kj~} -AE:GIR@N[_J\\eh";

void unpack_full(int phase, int piece, double scale, int offset) {
    for (int rank = 10; rank < 70; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | BLACK][70-rank+file] += v * phase;
        }
    }
}

void unpack_smol(int phase, int piece, double scale, int offset) {
    for (int rank = 0; rank < 80; rank+=20) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | WHITE][rank+file+1] += v * phase;
            PST[piece | WHITE][rank+file+10] += v * phase;
            PST[piece | WHITE][rank+file+11] += v * phase;
            PST[piece | BLACK][70-rank+file] += -v * phase;
            PST[piece | BLACK][71-rank+file] += -v * phase;
            PST[piece | BLACK][60-rank+file] += -v * phase;
            PST[piece | BLACK][61-rank+file] += -v * phase;
        }
    }
}

void unpack_half(
    int phase, int piece, double scale, int qll, int qlr, int qrl, int qrr
) {
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*DATA_STRING++ - ' ') * scale;
            PST[piece | WHITE][rank+file] += (v + qll) * phase;
            PST[piece | WHITE][7+rank-file] += (v + qrl) * phase;
            PST[piece | WHITE][70-rank+file] += (v + qlr) * phase;
            PST[piece | WHITE][77-rank-file] += (v + qrr) * phase;
            PST[piece | BLACK][rank+file] += (-v - qlr) * phase;
            PST[piece | BLACK][7+rank-file] += (-v - qrr) * phase;
            PST[piece | BLACK][70-rank+file] += (-v - qll) * phase;
            PST[piece | BLACK][77-rank-file] += (-v - qrl) * phase;
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

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    unpack_full(1, PAWN, 1.158, 29); // average: 70
    unpack_full(0x10000, PAWN, 1.0, 65); // average: 93
    unpack_full(1, PASSED_PAWN, 1.059, -20); // average: 12
    unpack_full(0x10000, PASSED_PAWN, 1.311, -19); // average: 32
    unpack_smol(1, KING, 1.0, -53); // average: -15
    unpack_smol(0x10000, KING, 1.0, -25); // average: 3
    unpack_half(1, QUEEN, 1.0, 519, 514, 520, 541); // average: 532
    unpack_half(0x10000, QUEEN, 1.0, 861, 915, 874, 919); // average: 891
    unpack_half(1, ROOK, 1.0, 287, 315, 297, 336); // average: 299
    unpack_half(0x10000, ROOK, 1.0, 436, 452, 429, 439); // average: 440
    unpack_half(1, BISHOP, 1.0, 225, 235, 227, 242); // average: 248
    unpack_half(0x10000, BISHOP, 1.0, 243, 249, 245, 247); // average: 257
    unpack_half(1, KNIGHT, 1.234, 139, 166, 147, 174); // average: 225
    unpack_half(0x10000, KNIGHT, 1.0, 193, 194, 192, 191); // average: 236
    
    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST.pieces[i][j] = rng();
        }
    }
    for (int i = 0; i < 120; i++) {
        ZOBRIST.ep[i] = rng();
    }
    ZOBRIST.castle_rights[0] = rng();
    ZOBRIST.castle_rights[1] = rng();
    ZOBRIST.castle_rights[2] = rng();
    ZOBRIST.castle_rights[3] = rng();
    ZOBRIST.stm = rng();
#else
    auto rng = fopen("/dev/urandom", "r");
    fread(&ZOBRIST, sizeof(ZOBRIST), 1, rng);
#endif
}
