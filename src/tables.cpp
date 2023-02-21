int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {7, -9, 12, 15, 13, 7, -9, 3};
int16_t DOUBLED_EG[] = {32, 25, 21, 16, 16, 24, 27, 38};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {16, 21, 20, 21};
int16_t PAWN_SHIELD_EG[] = {-28, -39, -25, -12};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 28
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 17
#define KING_OPEN_MG -38
#define KING_OPEN_EG -4
#define KING_SEMIOPEN_MG -16
#define KING_SEMIOPEN_EG 26
#define KING_ADJ_OPEN_MG -16
#define KING_ADJ_OPEN_EG -2
#define KING_ADJ_SEMIOPEN_MG -14
#define KING_ADJ_SEMIOPEN_EG 20

const char *DATA_STRING = "FZYB=D>7IWKGDH=<HOUWTPA?PXbc_YOH[bwvpiYOD g~xo:n$(3662+.  .-.(&$$*+'((+(,/1+00116E=AA==9aIn~zurw/832/**0,.&(\" */.-%\"&%36.50034B=4*73=DKQQN?FV[~T$% !!\"*!(.'%\"$)&6:40)+02FG>7/28>XUN;5;GOe~[7/5CBO>@]5 \"3KSJ7y{aD&40 6PK3WffUWji[ '),.26245422-219.$ ,.,4,CNK2NWb039? )22*512('01 &%!((&'$$(&'.,)#  !-73..<9611;D /&-*3772=EG,AGJ #*.)-94,?=E;8HG BTXL[aiVhrx_qz~";

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
    unpack_full(0, PAWN, 1.1, 5); // average: 61
    unpack_full(1, PAWN, 1.333, 97); // average: 131
    unpack_full(0, PASSED_PAWN, 1.254, -15); // average: 14
    unpack_full(1, PASSED_PAWN, 1.988, -5); // average: 41
    unpack_smol(0, KING, 1.0, -39); // average: 1
    unpack_smol(1, KING, 1.0, -38); // average: 4
    unpack_half(0, QUEEN, 1.0, 641, 642, 644, 658); // average: 656
    unpack_half(1, QUEEN, 1.0, 1275, 1330, 1269, 1334); // average: 1301
    unpack_half(0, ROOK, 1.0, 282, 315, 287, 326); // average: 297
    unpack_half(1, ROOK, 1.0, 629, 653, 622, 646); // average: 636
    unpack_half(0, BISHOP, 1.0, 238, 244, 237, 251); // average: 254
    unpack_half(1, BISHOP, 1.0, 369, 378, 369, 377); // average: 391
    unpack_half(0, KNIGHT, 1.0, 223, 242, 226, 247); // average: 244
    unpack_half(1, KNIGHT, 1.244, 265, 274, 265, 276); // average: 344
    
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
