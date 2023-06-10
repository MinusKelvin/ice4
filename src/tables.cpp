#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(23, 41)
int DOUBLED_PAWN[] = {S(8, 30), S(-8, 22), S(16, 15), S(18, 10), S(19, 8), S(14, 17), S(-4, 21), S(6, 35)};
#define TEMPO S(6, 2)
#define ISOLATED_PAWN S(8, 11)
int PROTECTED_PAWN[] = {0, S(7, 7), S(9, 4)};
#define ROOK_OPEN S(29, 11)
#define ROOK_SEMIOPEN S(16, 16)
int PAWN_SHIELD[] = {S(10, -25), S(19, -37), S(20, -25), S(25, -14)};
#define KING_OPEN S(-30, -5)
#define KING_SEMIOPEN S(-9, 19)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "EY\\@<A93HVNFCD98HO[XTM=;OXgc`VKE[c{vqgVK@ i~vl7h%(0642,.! ,,,('%%)'&%(,(-/.+-0216C9??=>:aHn~yurz.932.+*0,-%&  +/.-#!$&46-6/025C=3*63<DKQSNAGW\\~V#$  ##+\"'-'%$&*'694/+,13EF>6038?XUO;6<GPe~Z5/5DBO>@_6 \"4KPG7wv[C&2. 5LI2VbcTVhg[ '),.26245423-209.% -/,4,CNK2MWc139? )21*512('01 &&\"()&($%(''.,)#  !-73..<8611:D 0&-+3872=EG,AGJ #).)-94+?=E;8HG BUXL\\biVirx_qz~";

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
#else
FILE *RNG_FILE;
#endif

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    unpack_full(1, PAWN, 1.088, 10); // average: 64
    unpack_full(0x10000, PAWN, 1.337, 93); // average: 126
    unpack_full(1, PASSED_PAWN, 1.251, -14); // average: 14
    unpack_full(0x10000, PASSED_PAWN, 1.979, -5); // average: 40
    unpack_smol(1, KING, 1.0, -38); // average: 2
    unpack_smol(0x10000, KING, 1.0, -39); // average: 2
    unpack_half(1, QUEEN, 1.0, 640, 640, 643, 656); // average: 655
    unpack_half(0x10000, QUEEN, 1.0, 1274, 1329, 1268, 1333); // average: 1300
    unpack_half(1, ROOK, 1.0, 280, 313, 286, 324); // average: 296
    unpack_half(0x10000, ROOK, 1.0, 629, 653, 622, 646); // average: 636
    unpack_half(1, BISHOP, 1.0, 237, 243, 236, 250); // average: 253
    unpack_half(0x10000, BISHOP, 1.0, 368, 378, 369, 377); // average: 391
    unpack_half(1, KNIGHT, 1.0, 222, 241, 225, 246); // average: 243
    unpack_half(0x10000, KNIGHT, 1.241, 265, 275, 265, 276); // average: 343
    
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
    RNG_FILE = fopen("/dev/urandom", "r");
    fread(&ZOBRIST, sizeof(ZOBRIST), 1, RNG_FILE);
#endif
}
