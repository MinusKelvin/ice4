int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {7, -8, 16, 18, 19, 13, -4, 6};
int16_t DOUBLED_EG[] = {30, 22, 15, 10, 8, 16, 21, 35};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {11, 20, 21, 25};
int16_t PAWN_SHIELD_EG[] = {-25, -37, -25, -13};
int16_t ROOK_PSEUDO_MOBILITY_MG[] = {0, 4, 6, 6, 8};
int16_t ROOK_PSEUDO_MOBILITY_EG[] = {0, 48, 67, 75, 80};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 10
#define ROOK_OPEN_MG 27
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 15
#define ROOK_SEMIOPEN_EG 14
#define KING_OPEN_MG -30
#define KING_OPEN_EG -5
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 19

const char *DATA_STRING = "ARU<9=62CPHA?@54CITQNG97IQ^[XPF?T[olg^OF? e~xk9g&*3;96/3! ./.*(&&+)'&*/*/20-0365;K?FECD@fTt~tw|u.:43/++1-.%&  ,0/.#!$&57.70036E>4+84=FNSRR>=MV~P$$  ##+\"(-(%$&*'6940,-13FG?6139@YVP<7=HPk~`A>?JQP>@_7 \"4LPH7zw\\D&2. 5LI2VbcSVgg[ )*,.16245312-1/8,# +.+3+BMJ1MVb149@ )21*401(&./(%'!(#!#$ $\"'*(%\"! !.73--;8600:C 0&.*3882=DG,AFJ &*/)-94+>=E:7GF KUYL\\biVhqx_qz~";

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
            black_section[70-rank+file] = v;
            black_section[71-rank+file] = v;
            black_section[60-rank+file] = v;
            black_section[61-rank+file] = v;
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
            white_section[rank+file] = black_section[70-rank+file] = v + qll;
            white_section[7+rank-file] = black_section[77-rank-file] = v + qrl;
            white_section[70-rank+file] = black_section[rank+file] = v + qlr;
            white_section[77-rank-file] = black_section[7+rank-file] = v + qrr;
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
    unpack_full(0, PAWN, 1.236, 11); // average: 66
    unpack_full(1, PAWN, 1.094, 93); // average: 123
    unpack_full(0, PASSED_PAWN, 1.19, -14); // average: 13
    unpack_full(1, PASSED_PAWN, 1.945, -6); // average: 43
    unpack_smol(0, KING, 1.0, -41); // average: -1
    unpack_smol(1, KING, 1.0, -37); // average: 4
    unpack_half(0, QUEEN, 1.0, 638, 638, 641, 654); // average: 653
    unpack_half(1, QUEEN, 1.0, 1264, 1319, 1258, 1323); // average: 1289
    unpack_half(0, ROOK, 1.0, 281, 312, 287, 324); // average: 296
    unpack_half(1, ROOK, 1.0, 548, 572, 543, 565); // average: 553
    unpack_half(0, BISHOP, 1.0, 240, 246, 239, 253); // average: 256
    unpack_half(1, BISHOP, 1.0, 364, 373, 364, 372); // average: 386
    unpack_half(0, KNIGHT, 1.0, 224, 243, 228, 249); // average: 245
    unpack_half(1, KNIGHT, 1.232, 261, 271, 261, 272); // average: 340
    
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
