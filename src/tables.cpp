int16_t PST[2][25][SQUARE_SPAN];
int16_t CLOSE_PIECE_MG[] = {0, 14, 42, 31, 31, 36, 0};
int16_t CLOSE_PIECE_EG[] = {0, -25, -30, -20, -17, 17, 0};
int16_t DOUBLED_MG[] = {9, -8, 16, 18, 20, 16, -2, 7};
int16_t DOUBLED_EG[] = {31, 22, 15, 9, 8, 14, 19, 34};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 6};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 9
#define ISOLATED_PAWN_EG 10
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 10
#define ROOK_SEMIOPEN_MG 17
#define ROOK_SEMIOPEN_EG 16

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
    unpack_full(0, PAWN, "        NchLEH@:P_XPJK?@PWba[SDCU_mje]RLYayuqhZPE i~yp:o        ", 1.086, 2); // average: 48
    unpack_full(1, PAWN, "        $&-21/+-! ***'&%%)(&%(+(/11./121=KCHEB@<`Pn~uqor        ", 1.515, 94); // average: 123
    unpack_full(0, PASSED_PAWN, "        .921/.-2,.%&!#.1-*\" %(68-3-.15C=7,73<ELRQN@FU\\~V        ", 1.342, -17); // average: 6
    unpack_full(1, PASSED_PAWN, "        $$ !$$+\")/)'&'+'9>83..24HKC:35:@YVP=7<GPf~\\726CB        ", 1.929, -8); // average: 28
    unpack_smol(0, KING, "E36W1 $1EG?2roUB", 1.0, -31); // average: 3
    unpack_smol(1, KING, "(1- 1=:.NQQJQ^^U", 1.0, -31); // average: 3
    unpack_half(0, QUEEN, " ()--26235425/31", 1.0, 643, 639, 646, 652); // average: 658
    unpack_half(1, QUEEN, "<0% 01+3,BLK4LWd", 1.0, 1273, 1328, 1266, 1323); // average: 1300
    unpack_half(0, ROOK, "039@ )20*4/0)(01", 1.0, 282, 312, 287, 318); // average: 297
    unpack_half(1, ROOK, " &%!**'*#%)(&.-*", 1.0, 628, 655, 622, 650); // average: 635
    unpack_half(0, BISHOP, "#  !-73.-;8634;E", 1.0, 238, 239, 237, 243); // average: 254
    unpack_half(1, BISHOP, " /&-*3782>FI+?IL", 1.0, 370, 382, 370, 383); // average: 392
    unpack_half(0, KNIGHT, " $*.)/:5,>>E<;IH", 1.0, 221, 236, 224, 239); // average: 242
    unpack_half(1, KNIGHT, " BSWJY`hUhqx]oz~", 1.253, 269, 282, 269, 285); // average: 346
    
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
