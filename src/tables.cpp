int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -7, 16, 19, 20, 16, -3, 7};
int16_t DOUBLED_EG[] = {28, 19, 12, 6, 5, 12, 18, 32};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 5};
int16_t PAWN_SHIELD_MG[] = {-18, -2, 2, 7};
int16_t PAWN_SHIELD_EG[] = {12, -20, -11, -3};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 10
#define ROOK_SEMIOPEN_MG 17
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

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    unpack_full(0, PAWN, "        BXZ?8>6/EUKE?A55FNZYRJ:8NWgd^THBZc{woeSI> i~tk5g        ", 1.056, 15); // average: 53
    unpack_full(1, PAWN, "        ')1751,.# -,,'&%&('%$'+(-.-*,/227B9??<>;`Bk~xtnx        ", 1.33, 91); // average: 115
    unpack_full(0, PASSED_PAWN, "        .:31/,+1,/%&!!,0..# $'57-6//36D=3*63=ELRRL?GY]~V        ", 1.236, -15); // average: 7
    unpack_full(1, PASSED_PAWN, "        #$ !#%,$'-'%$'+(583/,-23CE=5039?VTN;6<GNc~Y506GC        ", 2.073, -6); // average: 30
    unpack_smol(0, KING, "Q?A`6 \"3JOH6vrXD", 1.0, -40); // average: -1
    unpack_smol(1, KING, "&/, 8LI5YbcWZjj_", 1.0, -36); // average: 6
    unpack_half(0, QUEEN, " '),.26245422-10", 1.0, 639, 640, 642, 656); // average: 654
    unpack_half(1, QUEEN, "8.$ ,.+3,CMK2MWb", 1.0, 1272, 1329, 1267, 1333); // average: 1298
    unpack_half(0, ROOK, "139? )11*512('01", 1.0, 279, 311, 284, 322); // average: 294
    unpack_half(1, ROOK, " &&\"**')$%(''.,)", 1.0, 628, 654, 622, 646); // average: 636
    unpack_half(0, BISHOP, "#  !-73..<8611;D", 1.0, 237, 242, 236, 249); // average: 253
    unpack_half(1, BISHOP, " 0&-+3772=EG,AGJ", 1.0, 368, 378, 368, 377); // average: 391
    unpack_half(0, KNIGHT, " #).)-94+>=E;8HF", 1.0, 221, 240, 225, 245); // average: 242
    unpack_half(1, KNIGHT, " CUXL\\aiVirx_qz~", 1.235, 265, 276, 265, 277); // average: 343
    
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
