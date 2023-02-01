int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -7, 16, 19, 20, 16, -3, 7};
int16_t DOUBLED_EG[] = {29, 20, 13, 6, 6, 13, 18, 33};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {7, 18, 21, 27};
int16_t PAWN_SHIELD_EG[] = {-23, -36, -26, -16};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 17
#define ROOK_SEMIOPEN_EG 14
#define CONNECTED_PASSED_MG 4
#define CONNECTED_PASSED_EG 5

const char *DATA_STRING = "BXZ?8=6/EULE?A55FNZXRJ:8MWgd]THBZb{wodSI? i~sk5f')1652-/\" ,,,('%&)'%%(,)./-*-0227C9??=>;`Ck~ytoy.931/,*1,.%%!!+0.-# $'47-6//36D=3*63=FLRRL@GZ]~W$$ !$%-$'-'&$'+(5830,-24DE=5039?VSM;6<GOc~Y606GCO=?_6 \"3IME3uqVB&0, 5HF2V^`SWgg\\ '),.26245422-108.$ ,.+3,BMK2MWb139? )11*502('01 &&\"))'($%)'(.-*#  !-73..<8610:D 0&.+3882=EG-AGJ #).(-94+>=E;7HF CUXL\\biVirx_qz~";

void unpack_full(int phase, int piece, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 10; rank < 70; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            black_section[70-rank+file] = -v;
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
    unpack_full(0, PAWN, 1.057, 15); // average: 65
    unpack_full(1, PAWN, 1.336, 90); // average: 123
    unpack_full(0, PASSED_PAWN, 1.235, -15); // average: 14
    unpack_full(1, PASSED_PAWN, 2.068, -9); // average: 39
    unpack_smol(0, KING, 1.0, -40); // average: -2
    unpack_smol(1, KING, 1.0, -38); // average: 2
    unpack_half(0, QUEEN, 1.0, 639, 639, 642, 655); // average: 654
    unpack_half(1, QUEEN, 1.0, 1274, 1331, 1269, 1335); // average: 1301
    unpack_half(0, ROOK, 1.0, 278, 311, 284, 322); // average: 294
    unpack_half(1, ROOK, 1.0, 630, 655, 623, 647); // average: 637
    unpack_half(0, BISHOP, 1.0, 237, 242, 236, 249); // average: 253
    unpack_half(1, BISHOP, 1.0, 368, 378, 369, 377); // average: 391
    unpack_half(0, KNIGHT, 1.0, 221, 240, 225, 245); // average: 242
    unpack_half(1, KNIGHT, 1.239, 265, 276, 265, 276); // average: 344
    
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
