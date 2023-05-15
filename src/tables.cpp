int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -8, 16, 18, 19, 14, -4, 6};
int16_t DOUBLED_EG[] = {30, 22, 15, 10, 8, 17, 21, 35};
int16_t PROTECTED_PAWN_MG[] = {0, 14, 17};
int16_t PROTECTED_PAWN_EG[] = {0, 14, 8};
int16_t PAWN_SHIELD_MG[] = {20, 38, 41, 49};
int16_t PAWN_SHIELD_EG[] = {-50, -73, -50, -28};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 46
#define BISHOP_PAIR_EG 83
#define TEMPO_MG 12
#define TEMPO_EG 4
#define ISOLATED_PAWN_MG 17
#define ISOLATED_PAWN_EG 21
#define ROOK_OPEN_MG 57
#define ROOK_OPEN_EG 22
#define ROOK_SEMIOPEN_MG 32
#define ROOK_SEMIOPEN_EG 33
#define KING_OPEN_MG -60
#define KING_OPEN_EG -11
#define KING_SEMIOPEN_MG -18
#define KING_SEMIOPEN_EG 37

const char *DATA_STRING = "EY\\@<A93HVNFCD98HO[XTM=;OXgc`VKE[c{vqgVK@ i~vl7h%(0642,.! ,,,('%%)'&%(,(-/.+-0216C9??=>:aHn~yurz.932.+*0,-%&  +/.-#!$&46-6/025C=3*63<DKQSNAGW\\~V#$  ##+\"'-'%$&*'694/+,13EF>6038?XUO;6<GPe~Z5/5DBS@Bd8 \"6OTJ9~|_F(82 <ZU8gwxdg~}n /39<CLDHJHDE:C?C4' 250;1Q`\\:_m~AFQ^ 2DB4JBD1.@B ,,#12-0(*0-.<93& !!:NE;;XQLBAUg @+;5FONDZjo9bnu '3<1;RI7][kUOpm BUXL\\biVirx_qz~";

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
    unpack_full(0, PAWN, 2.176, 20); // average: 129
    unpack_full(1, PAWN, 2.675, 186); // average: 253
    unpack_full(0, PASSED_PAWN, 2.501, -29); // average: 28
    unpack_full(1, PASSED_PAWN, 3.958, -11); // average: 81
    unpack_smol(0, KING, 1.852, -76); // average: 4
    unpack_smol(1, KING, 1.527, -77); // average: 5
    unpack_half(0, QUEEN, 1.0, 1279, 1281, 1286, 1313); // average: 1310
    unpack_half(1, QUEEN, 1.429, 2547, 2659, 2536, 2667); // average: 2600
    unpack_half(0, ROOK, 1.0, 561, 626, 571, 649); // average: 592
    unpack_half(1, ROOK, 1.0, 1258, 1307, 1245, 1292); // average: 1272
    unpack_half(0, BISHOP, 1.0, 474, 486, 473, 500); // average: 506
    unpack_half(1, BISHOP, 1.0, 737, 755, 737, 754); // average: 782
    unpack_half(0, KNIGHT, 1.0, 444, 481, 450, 492); // average: 485
    unpack_half(1, KNIGHT, 2.482, 530, 550, 530, 551); // average: 687
    
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
