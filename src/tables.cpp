int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {11, -9, 17, 20, 22, 15, -6, 11};
int16_t DOUBLED_EG[] = {32, 25, 19, 12, 9, 19, 24, 36};
int16_t PROTECTED_PAWN_MG[] = {0, 8, 10};
int16_t PROTECTED_PAWN_EG[] = {0, 6, 3};
int16_t PAWN_SHIELD_MG[] = {10, 21, 23, 28};
int16_t PAWN_SHIELD_EG[] = {-21, -39, -27, -16};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 24
#define BISHOP_PAIR_EG 47
#define TEMPO_MG 7
#define TEMPO_EG 1
#define ISOLATED_PAWN_MG 10
#define ISOLATED_PAWN_EG 9
#define ROOK_OPEN_MG 36
#define ROOK_OPEN_EG 8
#define ROOK_SEMIOPEN_MG 20
#define ROOK_SEMIOPEN_EG 17
#define KING_OPEN_MG -36
#define KING_OPEN_EG -4
#define KING_SEMIOPEN_MG -10
#define KING_SEMIOPEN_EG 21

const char *DATA_STRING = "?UY84<5+DTJ@=@32DKWTRI85MVea]RHA\\a|vqbSJC l~xd9q&*0963.0\" ,,,('&$(&$$'+(*--*,/003@9?>=;8^Jh~tuno0<440-,2--%%! -221\" %&8;0:0248GA:1;7>FOXUQBJXd~T$%! #$,\"(/)&$&+'6:62-/23GG@724:@XTN<7?JQf~^757GF@34J2! ,XNK?j~eJ)3. 4KI2DXZG0AH: ''+/28356533,2/:1( .30:0JWU7Ycq36=F .98+;79++65 ''#-)(+'%***0/.$# #2=911C?;75AJ *\"-\"/661;DH*AHK \"+0+0?7,FCN@;OM FW\\O_bkYjrxbr{~";

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
    unpack_full(0, PAWN, 1.169, 20); // average: 75
    unpack_full(1, PAWN, 1.424, 94); // average: 128
    unpack_full(0, PASSED_PAWN, 1.21, -16); // average: 15
    unpack_full(1, PASSED_PAWN, 1.973, -8); // average: 40
    unpack_smol(0, KING, 1.661, -45); // average: 17
    unpack_smol(1, KING, 1.0, -27); // average: 2
    unpack_half(0, QUEEN, 1.0, 782, 782, 786, 806); // average: 797
    unpack_half(1, QUEEN, 1.0, 1136, 1194, 1131, 1197); // average: 1169
    unpack_half(0, ROOK, 1.0, 338, 376, 344, 391); // average: 358
    unpack_half(1, ROOK, 1.0, 582, 603, 575, 595); // average: 591
    unpack_half(0, BISHOP, 1.0, 283, 290, 283, 299); // average: 304
    unpack_half(1, BISHOP, 1.0, 331, 338, 331, 338); // average: 352
    unpack_half(0, KNIGHT, 1.0, 262, 285, 266, 292); // average: 286
    unpack_half(1, KNIGHT, 1.496, 212, 219, 212, 220); // average: 310
    
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
