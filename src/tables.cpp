int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -8, 16, 18, 19, 14, -4, 6};
int16_t DOUBLED_EG[] = {30, 22, 15, 10, 8, 16, 21, 34};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {10, 19, 20, 24};
int16_t PAWN_SHIELD_EG[] = {-25, -36, -25, -14};
int16_t KNIGHT_MOBILITY_MG[] = {0, 9, 11, 13, 14, 13, 13, 14, 16};
int16_t KNIGHT_MOBILITY_EG[] = {0, 133, 143, 150, 151, 153, 152, 150, 143};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
int STARTS[] = {0,0,8,4,0,0,0};
int LIMITS[] = {0,0,1,8,8,8,1};
int ENDS[] = {0,0,16,8,4,8,8};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 29
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -30
#define KING_OPEN_EG -5
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 19

const char *DATA_STRING = "ASV>;>71DQJB@A77EKUROH;8KR`\\YQGAU\\qmi_PG@ e~wl9i&*3:86/1! ./.)(&&+)'&*.*/20-0365;K>EEBD@eRt~uxzs/:42/,+1-.%&! ,0/.$!$&58.70136E>4+74=GNSRQ>>OW~P#$  ##+\"'-'$$&*'694/+,13FG?6039?YUO;6<GPk~_@<>JQP>@_6 \"4KOG7xu[C&2. 5LI2VccTVhg[ '),.26245522-209.% -/,3,CMK3NWc038? )21+512('01 &&!()&($%(''.,)# !!-73..<9611;D 0&-+3772=DG,AGJ  &+%)4/'98?63B@ ;KPBSXaL`nvVjy~";

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
    unpack_full(0, PAWN, 1.232, 9); // average: 66
    unpack_full(1, PAWN, 1.106, 93); // average: 124
    unpack_full(0, PASSED_PAWN, 1.199, -15); // average: 13
    unpack_full(1, PASSED_PAWN, 1.951, -5); // average: 43
    unpack_smol(0, KING, 1.0, -40); // average: -1
    unpack_smol(1, KING, 1.0, -38); // average: 3
    unpack_half(0, QUEEN, 1.0, 639, 639, 642, 655); // average: 654
    unpack_half(1, QUEEN, 1.0, 1269, 1325, 1263, 1329); // average: 1295
    unpack_half(0, ROOK, 1.0, 281, 314, 287, 325); // average: 297
    unpack_half(1, ROOK, 1.0, 626, 651, 620, 643); // average: 633
    unpack_half(0, BISHOP, 1.0, 238, 243, 237, 250); // average: 254
    unpack_half(1, BISHOP, 1.0, 366, 376, 367, 375); // average: 389
    unpack_half(0, KNIGHT, 1.0, 214, 232, 218, 238); // average: 231
    unpack_half(1, KNIGHT, 1.032, 135, 145, 135, 146); // average: 193
    
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
