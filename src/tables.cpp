int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {9, -5, 15, 18, 21, 15, -3, 7};
int16_t DOUBLED_EG[] = {33, 23, 18, 12, 9, 17, 21, 38};
int16_t PROTECTED_PAWN_MG[] = {0, 8, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 5};
int16_t PAWN_SHIELD_MG[] = {9, 19, 21, 26};
int16_t PAWN_SHIELD_EG[] = {-24, -36, -25, -16};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 43
#define TEMPO_MG 8
#define TEMPO_EG 5
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -34
#define KING_OPEN_EG -4
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 18

const char *DATA_STRING = "@TW;7<6.BQH@=>53DLUROG:7LTb^ZQGAW[umlaPG< Sz~Y*`'*2:65-0\" --,('&&*(&&)-*.00,/234;H=FEAC>cHn{x~pj.941-*+/-+%# !)...$ $%35.8.025B;7276<BLNPPIBI`~U##  \"\")!&,'%$%*%483/++01CD=5/17<TQM:4;CLd~^A75JP]IKi; \"<JH@9~i\\h&2- 9OK3ZgfVWpjV &'+.25234411-1/0)# &(*2(>IL0JV_/28> )20)513''./ &&#''%(##'&',+*(\"  /73..>9722<D .$*(0650:BE*?EH %*0+/<6/A@H>;KI CUVL\\bjWirx_qz~";

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
    unpack_full(0, PAWN, 1.158, 13); // average: 64
    unpack_full(1, PAWN, 1.204, 93); // average: 125
    unpack_full(0, PASSED_PAWN, 1.335, -16); // average: 14
    unpack_full(1, PASSED_PAWN, 2.038, -4); // average: 43
    unpack_smol(0, KING, 1.002, -49); // average: -5
    unpack_smol(1, KING, 1.0, -38); // average: 5
    unpack_half(0, QUEEN, 1.0, 657, 660, 659, 675); // average: 671
    unpack_half(1, QUEEN, 1.0, 1248, 1306, 1245, 1312); // average: 1271
    unpack_half(0, ROOK, 1.0, 282, 317, 287, 328); // average: 297
    unpack_half(1, ROOK, 1.0, 622, 645, 616, 639); // average: 629
    unpack_half(0, BISHOP, 1.0, 239, 245, 238, 251); // average: 256
    unpack_half(1, BISHOP, 1.0, 365, 373, 365, 373); // average: 385
    unpack_half(0, KNIGHT, 1.0, 219, 239, 223, 244); // average: 242
    unpack_half(1, KNIGHT, 1.234, 261, 270, 260, 271); // average: 339
    
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
