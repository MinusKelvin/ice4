#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(21, 44)
int32_t DOUBLED_PAWN[] = {S(5, 16), S(-3, 11), S(8, 17), S(10, 20), S(11, 9), S(13, 16), S(-5, 12), S(14, 22)};
#define TEMPO S(12, 8)
#define ISOLATED_PAWN S(7, 8)
int32_t PROTECTED_PAWN[] = {0, S(8, 6), S(10, 6)};
#define ROOK_OPEN S(35, 6)
#define ROOK_SEMIOPEN S(15, 13)
int32_t PAWN_SHIELD[] = {S(2, -11), S(14, -30), S(17, -25), S(25, -20)};
#define KING_OPEN S(-45, 2)
#define KING_SEMIOPEN S(-11, 17)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "7HF.(.+ 9A7622(#<=EDA7.(CETOF?92VUr]VTA>6 f~n_>[#(5J;827 #368.*,\"*)+,-0,10-+.457=O9HOCHHHVV\\QfaY.A23'.2;65,!%(,D+6' )/;F28=7?IGG#78;HKRUU[LIgRhI4.+ ,(4/7:13-0:2KGA>65=CURKE=>FPj]aD=P]_~oeDMEWdmRNpC (@D!,>MNTy*94(;NI6@VR?&89  '',6381;6501221#'11 +6A&ANS6GS_-/28 /1,(9.-''*)&'+&'('($ %%%))()<& 6AE47BA@46AQ!$ + '%1,.68'6:7 WYX\\_pk_ur~lh~}  5:/;>D8@MP;OTX";

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
    unpack_full(1, PAWN, 1.24, 23); // average: 65
    unpack_full(0x10000, PAWN, 1.0, 60); // average: 86
    unpack_full(1, PASSED_PAWN, 1.0, -19); // average: 9
    unpack_full(0x10000, PASSED_PAWN, 1.262, -18); // average: 29
    unpack_smol(1, KING, 1.0, -59); // average: -19
    unpack_smol(0x10000, KING, 1.0, -24); // average: 2
    unpack_half(1, QUEEN, 1.0, 483, 482, 485, 504); // average: 499
    unpack_half(0x10000, QUEEN, 1.0, 788, 832, 796, 838); // average: 815
    unpack_half(1, ROOK, 1.0, 262, 287, 271, 309); // average: 274
    unpack_half(0x10000, ROOK, 1.0, 390, 405, 383, 392); // average: 397
    unpack_half(1, BISHOP, 1.0, 202, 212, 205, 218); // average: 227
    unpack_half(0x10000, BISHOP, 1.0, 217, 223, 217, 221); // average: 229
    unpack_half(1, KNIGHT, 1.154, 122, 149, 130, 157); // average: 202
    unpack_half(0x10000, KNIGHT, 1.0, 182, 182, 179, 178); // average: 212
    
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
