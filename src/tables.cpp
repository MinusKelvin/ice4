int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {10, -5, 16, 18, 21, 16, -3, 7};
int16_t DOUBLED_EG[] = {32, 22, 17, 11, 9, 18, 21, 38};
int16_t PROTECTED_PAWN_MG[] = {0, 8, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 8, 5};
int16_t PAWN_SHIELD_MG[] = {8, 17, 20, 25};
int16_t PAWN_SHIELD_EG[] = {-23, -36, -25, -16};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 21
#define BISHOP_PAIR_EG 45
#define TEMPO_MG 7
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 28
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -31
#define KING_OPEN_EG -6
#define KING_SEMIOPEN_MG -8
#define KING_SEMIOPEN_EG 18

const char *DATA_STRING = "AUX<8=70CRIA>@64DMWTQI<8MUd`]SHCX]xqndRI< W|~V+f')1753-/\" -,,('%&*(&&)-*.//,.1239E:CA>@<bChqz~kj,610-)+/,+%$ !)...$ $&35.8.025C;6145<AKMOKGAMe~Q#$  \"#*!','$#%*%594/+,01CE>5/17=URN94;CMb~[>,.GKQ>@]7 \"5PPH4~yka&3. 7MJ2XfdUUkfV '(+.25234412-0.9-$ ++-4,BLL1MYc/38> (20)513''// %%\"'(&(##(''-+*)\"  /72-.=8621<C /$*)0641:AE*>DG %+/+/;6.@@H=:KI DVWK\\biWjrx_rz~";

void unpack_full(int phase, int piece, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 10; rank < 70; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            black_section[70-rank+file] = v;
        }
    }
}

void unpack_smol(int phase, int piece, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=20) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
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
    int phase, int piece, double scale, int qll, int qlr, int qrl, int qrr
) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*DATA_STRING++ - ' ') * scale;
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

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    unpack_full(0, PAWN, 1.123, 13); // average: 64
    unpack_full(1, PAWN, 1.317, 94); // average: 128
    unpack_full(0, PASSED_PAWN, 1.347, -16); // average: 14
    unpack_full(1, PASSED_PAWN, 2.005, -3); // average: 42
    unpack_smol(0, KING, 1.009, -41); // average: 3
    unpack_smol(1, KING, 1.0, -39); // average: 3
    unpack_half(0, QUEEN, 1.0, 671, 672, 673, 686); // average: 685
    unpack_half(1, QUEEN, 1.0, 1283, 1340, 1278, 1343); // average: 1309
    unpack_half(0, ROOK, 1.0, 292, 326, 296, 336); // average: 307
    unpack_half(1, ROOK, 1.0, 638, 662, 632, 655); // average: 644
    unpack_half(0, BISHOP, 1.0, 246, 252, 245, 258); // average: 263
    unpack_half(1, BISHOP, 1.0, 374, 382, 374, 382); // average: 395
    unpack_half(0, KNIGHT, 1.0, 226, 245, 229, 250); // average: 248
    unpack_half(1, KNIGHT, 1.225, 271, 280, 270, 280); // average: 348
    
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
