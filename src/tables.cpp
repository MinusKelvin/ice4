int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {7, -8, 15, 17, 18, 13, -4, 6};
int16_t DOUBLED_EG[] = {29, 22, 15, 10, 8, 16, 21, 34};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 8};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {0, 8, 10, 13};
int16_t PAWN_SHIELD_EG[] = {-19, -31, -19, -8};
int16_t MOBILITY_MG[] = {0, 1, 1, 2, 1, 0, -5};
int16_t MOBILITY_EG[] = {0, 1, -5, 2, 1, 2, 2};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
int STARTS[] = {0,0,8,4,0,0,0};
int LIMITS[] = {0,0,1,8,8,8,1};
int ENDS[] = {0,0,16,8,4,8,8};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 43
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 10
#define ROOK_OPEN_MG 27
#define ROOK_OPEN_EG 8
#define ROOK_SEMIOPEN_MG 15
#define ROOK_SEMIOPEN_EG 15
#define KING_OPEN_MG -29
#define KING_OPEN_EG -6
#define KING_SEMIOPEN_MG -8
#define KING_SEMIOPEN_EG 18

const char *DATA_STRING = ">OQ96:5/ANG@=>44BHROLE86HO\\XUMC>QXmje\\MD@ d~yn<j%*3;85.0! ...)(%%+)''*.*/21-0365:K?FFBC?gTt~tvzr/;651,+2-.%(  ,0/.# #&78.70026G@6*84=HPXRO::KT~M$$  ##,#(-(%$&+'6:50,-24FG?7149@ZVP<7=IQi~`A=?KRM<>\\2 !0EOF0sv\\>&2. 6KH4XbbUWgf\\ '(*,/2.221.0*.,8,\" /-*2/BJG4LT]138? )11*502('01 %% **')&&)(*0.+%   +3-(,84//,3= .$+*04309>A*<@A $(-(,62+=;C:6ED ?QTGWbgRgtyYm{~";

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
    unpack_full(0, PAWN, 1.24, 10); // average: 64
    unpack_full(1, PAWN, 1.113, 93); // average: 124
    unpack_full(0, PASSED_PAWN, 1.069, -14); // average: 11
    unpack_full(1, PASSED_PAWN, 1.959, -7); // average: 43
    unpack_smol(0, KING, 1.0, -39); // average: -2
    unpack_smol(1, KING, 1.0, -34); // average: 7
    unpack_half(0, QUEEN, 1.0, 605, 605, 608, 621); // average: 617
    unpack_half(1, QUEEN, 1.0, 1257, 1313, 1252, 1318); // average: 1282
    unpack_half(0, ROOK, 1.0, 270, 303, 275, 313); // average: 285
    unpack_half(1, ROOK, 1.0, 616, 641, 611, 634); // average: 624
    unpack_half(0, BISHOP, 1.0, 226, 232, 225, 238); // average: 239
    unpack_half(1, BISHOP, 1.0, 361, 370, 361, 369); // average: 380
    unpack_half(0, KNIGHT, 1.0, 214, 232, 217, 237); // average: 233
    unpack_half(1, KNIGHT, 1.533, 275, 286, 275, 286); // average: 369
    
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
