#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(26, 47)
int DOUBLED_PAWN[] = {S(7, 28), S(-12, 25), S(12, 23), S(14, 16), S(17, 11), S(12, 23), S(-10, 25), S(8, 35)};
#define TEMPO S(10, 8)
#define ISOLATED_PAWN S(10, 8)
int PROTECTED_PAWN[] = {0, S(8, 7), S(9, 6)};
#define ROOK_OPEN S(36, 12)
#define ROOK_SEMIOPEN S(18, 16)
int PAWN_SHIELD[] = {S(11, -21), S(21, -39), S(23, -31), S(30, -24)};
#define KING_OPEN S(-43, -1)
#define KING_SEMIOPEN S(-11, 20)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "9MO0*2* =I@856)&?BNLH>/+HN^YRG>6Y_}jf^GB>\"f~qkPb%+6D<835  034+)(#)'%&+.+.0.*-566=N>JIEK@fipodoug5B88223;8/)''%/;25$ ))< 35 <7:=DNI0*;@FKYYYZFH]TiW('$ #%-%*4-(#)0+>C=60168QQH<78@Gic[B>JS_z~nQSRYchMNqK\" :dJEAf}ed$40 1ML2@Y[F/= 26  '(-13:587643231($)) (/7%BQP2LXg14:@ .73+;45)*11 %'#'%%&\" %$%**(#' #2=;10A=<24?K +%,!/3739DG,BFJ $.0,1B;/GGQ?>RR CU[P]ajWgqw`qz~";

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
    unpack_full(1, PAWN, 1.14, 27); // average: 72
    unpack_full(0x10000, PAWN, 1.0, 86); // average: 114
    unpack_full(1, PASSED_PAWN, 1.0, -19); // average: 10
    unpack_full(0x10000, PASSED_PAWN, 1.533, -8); // average: 42
    unpack_smol(1, KING, 1.0, -56); // average: -6
    unpack_smol(0x10000, KING, 1.0, -26); // average: 2
    unpack_half(1, QUEEN, 1.0, 563, 561, 565, 585); // average: 580
    unpack_half(0x10000, QUEEN, 1.0, 1080, 1138, 1089, 1143); // average: 1105
    unpack_half(1, ROOK, 1.0, 301, 329, 308, 351); // average: 319
    unpack_half(0x10000, ROOK, 1.0, 538, 560, 531, 548); // average: 543
    unpack_half(1, BISHOP, 1.0, 248, 258, 249, 265); // average: 268
    unpack_half(0x10000, BISHOP, 1.0, 298, 304, 300, 303); // average: 319
    unpack_half(1, KNIGHT, 1.0, 226, 251, 233, 259); // average: 253
    unpack_half(0x10000, KNIGHT, 1.346, 199, 204, 198, 203); // average: 285
    
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
