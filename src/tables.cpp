int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -8, 16, 18, 19, 14, -4, 6};
int16_t DOUBLED_EG[] = {30, 22, 15, 10, 8, 16, 21, 35};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 4};
int16_t PAWN_SHIELD_MG[] = {2, 11, 13, 17};
int16_t PAWN_SHIELD_EG[] = {-15, -29, -20, -11};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 29
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -30
#define KING_OPEN_EG -6
#define KING_SEMIOPEN_MG -9
#define KING_SEMIOPEN_EG 18

const char *DATA_STRING = "EZ\\@=A:3HWNFCE99IP[XUM>;PYhd`WLE[c|wrgVLA i~wm6h%(1742,.! ,,+'&%$)'%%(+(,.-*-/216C9>><=:aJl~wss{.932/+*0,-%&  +/.-#!$&46-60025C=3+64<DLQSN@GW\\~V$$  ##+\"'-'$$&*'694/+,13EF>5039?XUO;6<GPe~[506DA<55E>+1B. \"-+)%%684-;981CA8+~za\\).&#%=7 6NJ4CXVAVdeRjuvhjyxm*JK5 '*-.26245422-2/9/$ -/,4-DNL3NXd039? )21*512('01 &%!((&($%(''.,)# !!-73..<9611;D 0%.*2872=DG,AGJ #*.)-94+?=F;8HG CTXL\\bjVirx_qz~";

void unpack_pawn(int phase, int piece, double scale, int offset) {
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

void unpack_king(int phase, double scale, int offset) {
    int16_t *white_section = PST[phase][WHITE_KING];
    int16_t *black_section = PST[phase][BLACK_KING];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[rank+file+1] = v;
            black_section[70-rank+file] = -v;
            black_section[71-rank+file] = -v;
        }
    }
}

void unpack_piece(
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
    unpack_pawn(0, PAWN, 1.089, 10); // average: 64
    unpack_pawn(1, PAWN, 1.337, 93); // average: 126
    unpack_pawn(0, PASSED_PAWN, 1.255, -14); // average: 14
    unpack_pawn(1, PASSED_PAWN, 1.966, -5); // average: 40
    unpack_king(0, 1.928, -46); // average: 5
    unpack_king(1, 1.0, -40); // average: 3
    unpack_piece(0, QUEEN, 1.0, 640, 641, 644, 657); // average: 656
    unpack_piece(1, QUEEN, 1.0, 1272, 1328, 1267, 1332); // average: 1299
    unpack_piece(0, ROOK, 1.0, 280, 313, 286, 324); // average: 296
    unpack_piece(1, ROOK, 1.0, 629, 654, 623, 646); // average: 636
    unpack_piece(0, BISHOP, 1.0, 237, 243, 236, 250); // average: 253
    unpack_piece(1, BISHOP, 1.0, 368, 378, 369, 377); // average: 391
    unpack_piece(0, KNIGHT, 1.0, 222, 241, 225, 246); // average: 243
    unpack_piece(1, KNIGHT, 1.237, 265, 275, 265, 276); // average: 343
    
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
