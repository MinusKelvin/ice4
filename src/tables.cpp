#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(24, 45)
int32_t DOUBLED_PAWN[] = {S(9, 29), S(-11, 25), S(13, 21), S(16, 14), S(19, 10), S(13, 21), S(-8, 24), S(8, 36)};
#define TEMPO S(8, 4)
#define ISOLATED_PAWN S(9, 9)
int32_t PROTECTED_PAWN[] = {0, S(8, 6), S(9, 3)};
#define ROOK_OPEN S(35, 9)
#define ROOK_SEMIOPEN S(18, 15)
int32_t PAWN_SHIELD[] = {S(13, -22), S(22, -39), S(24, -29), S(30, -22)};
#define KING_OPEN S(-39, -2)
#define KING_SEMIOPEN S(-11, 20)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "9MQ3.5.$>KC;89-+?DPNKB3/GO^ZUI@:W\\vkeZJC; g{~j8h%*2>9602! .00)(&$)'%%),),..),2225E<ED@B<[^qsok~h3>7820083.'$%#-744$ '(:@3:569>LF5.:;CKVYZXCJJT~R''# \"$-\"*4,(#(.):A;50057MOE;57>Dd^V?;EOXu~bD?GFWVAB`?! 4aQGCm~lU&3. 1IH0@SVB+:@0 ')-/3;557643/20.(% '()5(@NL3NYg25<C /96+:57)+24 %'\")'%'#\"&&&,+)\"% #2<910@=;34@J )#,!-2508BE)?DH \"-1+0@9.EDN?=PO FV[P^akXirwar{~";

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
    unpack_full(1, PAWN, 1.193, 24); // average: 72
    unpack_full(0x10000, PAWN, 1.164, 85); // average: 116
    unpack_full(1, PASSED_PAWN, 1.061, -18); // average: 11
    unpack_full(0x10000, PASSED_PAWN, 1.572, -6); // average: 40
    unpack_smol(1, KING, 1.149, -48); // average: 4
    unpack_smol(0x10000, KING, 1.0, -24); // average: 2
    unpack_half(1, QUEEN, 1.0, 604, 604, 608, 628); // average: 621
    unpack_half(0x10000, QUEEN, 1.0, 1142, 1187, 1139, 1193); // average: 1166
    unpack_half(1, ROOK, 1.0, 312, 344, 319, 363); // average: 331
    unpack_half(0x10000, ROOK, 1.0, 549, 568, 541, 557); // average: 555
    unpack_half(1, BISHOP, 1.0, 261, 269, 261, 277); // average: 281
    unpack_half(0x10000, BISHOP, 1.0, 306, 312, 307, 311); // average: 325
    unpack_half(1, KNIGHT, 1.0, 240, 262, 245, 270); // average: 265
    unpack_half(0x10000, KNIGHT, 1.363, 200, 204, 199, 204); // average: 288
    
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
