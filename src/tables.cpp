int16_t PST[2][25][SQUARE_SPAN];
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {10, 19, 20, 24};
int16_t PAWN_SHIELD_EG[] = {-25, -37, -25, -14};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define DOUBLED_PAWN_MG 10
#define DOUBLED_PAWN_EG 18
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 7
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 29
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -30
#define KING_OPEN_EG -5
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 19

const char *DATA_STRING = "=TS7485+@QE=:;41AJRNKD93HT]XULH=S_pkf\\RD; d~xj:g%*4=:7.0  /10+(%%+*)(+.).12025549J@HHDC>gPs~svwv.:440,*0,.%(! +/.-#\"%'47.81147E?4,86?HNTUR?@PY~R%$! ##,$)-($$&+(7:4/+,24FG?5039@ZUO;6<HQi~`A=?KOO=@_6 \"4JOG6wuZA&2. 5LI2WbcTVhg\\ (),.26245422-209.% -/+4,CNK2MWc139? )21*512((01 &&\"()&($%(''.,)#   -73.-<8611:D 0&-+3772=EG-AGJ #*.)-94,?=E;8HG CUXL\\biVirx_qz~";

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
    unpack_full(0, PAWN, 1.123, 18); // average: 66
    unpack_full(1, PAWN, 1.129, 92); // average: 123
    unpack_full(0, PASSED_PAWN, 1.159, -15); // average: 12
    unpack_full(1, PASSED_PAWN, 1.977, -6); // average: 43
    unpack_smol(0, KING, 1.0, -40); // average: -1
    unpack_smol(1, KING, 1.0, -38); // average: 4
    unpack_half(0, QUEEN, 1.0, 638, 639, 641, 655); // average: 653
    unpack_half(1, QUEEN, 1.0, 1270, 1326, 1264, 1330); // average: 1296
    unpack_half(0, ROOK, 1.0, 281, 314, 286, 325); // average: 296
    unpack_half(1, ROOK, 1.0, 627, 651, 621, 644); // average: 634
    unpack_half(0, BISHOP, 1.0, 238, 243, 237, 250); // average: 253
    unpack_half(1, BISHOP, 1.0, 367, 376, 367, 375); // average: 390
    unpack_half(0, KNIGHT, 1.0, 222, 241, 225, 246); // average: 243
    unpack_half(1, KNIGHT, 1.24, 264, 274, 264, 274); // average: 342
    
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
