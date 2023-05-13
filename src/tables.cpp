int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -7, 15, 17, 18, 13, -3, 7};
int16_t DOUBLED_EG[] = {30, 23, 15, 10, 8, 17, 21, 35};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {14, 22, 23, 26};
int16_t PAWN_SHIELD_EG[] = {-25, -36, -23, -11};
int16_t ROOK_MOBILITY_MG[] = {0, 3, 6, 8, 8, 10, 11, 13, 12, 13, 14, 16, 17, 20, 19};
int16_t ROOK_MOBILITY_EG[] = {0, 41, 50, 61, 72, 79, 83, 85, 92, 95, 97, 98, 98, 96, 96};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
int STARTS[] = {0,0,8,4,0,0,0};
int LIMITS[] = {0,0,1,8,8,8,1};
int ENDS[] = {0,0,16,8,4,8,8};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 9
#define ISOLATED_PAWN_EG 10
#define ROOK_OPEN_MG 23
#define ROOK_OPEN_EG 4
#define ROOK_SEMIOPEN_MG 12
#define ROOK_SEMIOPEN_EG 11
#define KING_OPEN_MG -31
#define KING_OPEN_EG -4
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 19

const char *DATA_STRING = "EVY?<?95GSLDAB88FLWTQJ<9KTa^[RHAV^rnj`QG? f~wn9g%*3<85/1! ./.*(&&+)'&*/+020-/365;K?EDCD@hTt~vuzr.:43/+*1,.%'  ,0..# $&58.70026E>3+74<FNRRR=<MU~O$$  #$+\"(-(%$&*'6:50,-13FG@7149@YVP<7=HPj~aB=AKRP?@_8 \"6NSJ9||aG&30 6MJ3WcdUWhh\\ )+-.15134312,0.3( \"&)'/'=IG+HS_24;B (11(2./&$--!%% +*(*'&*()/-*!  !,51,,:64//8A 1-1*2881=EG-AGK '+0)-94+>=E:7GF MV[L\\biUhqx_qz~";

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
    unpack_full(0, PAWN, 1.19, 9); // average: 65
    unpack_full(1, PAWN, 1.095, 93); // average: 124
    unpack_full(0, PASSED_PAWN, 1.158, -14); // average: 13
    unpack_full(1, PASSED_PAWN, 1.954, -7); // average: 42
    unpack_smol(0, KING, 1.0, -40); // average: 2
    unpack_smol(1, KING, 1.0, -37); // average: 5
    unpack_half(0, QUEEN, 1.0, 639, 638, 642, 654); // average: 654
    unpack_half(1, QUEEN, 1.0, 1272, 1328, 1266, 1331); // average: 1294
    unpack_half(0, ROOK, 1.0, 284, 313, 290, 324); // average: 299
    unpack_half(1, ROOK, 1.0, 531, 554, 526, 546); // average: 539
    unpack_half(0, BISHOP, 1.0, 243, 247, 242, 254); // average: 257
    unpack_half(1, BISHOP, 1.0, 364, 373, 364, 372); // average: 388
    unpack_half(0, KNIGHT, 1.0, 225, 243, 229, 249); // average: 246
    unpack_half(1, KNIGHT, 1.243, 262, 271, 261, 271); // average: 341
    
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
