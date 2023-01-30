int16_t PST[2][25][SQUARE_SPAN];
int16_t TROPISM[2][25][10] = {
    {
        {},{},{},{},{},{},{},{},
        {},
        {0, 20, 22, 23, 22, 22, 21, 19},
        {0, 33, 29, 29, 27, 27, 28, 27},
        {0, 34, 36, 36, 36, 36, 37, 37},
        {0, 33, 31, 31, 29, 30, 36, 25},
        {0, 18, 21, 21, 19, 18, 18, 17},
        {0, 18, 16, 18, 18, 20, 28, 44},
        {},
        {},
        {0, -20, -22, -23, -22, -22, -21, -19},
        {0, -33, -29, -29, -27, -27, -28, -27},
        {0, -34, -36, -36, -36, -36, -37, -37},
        {0, -33, -31, -31, -29, -30, -36, -25},
        {0, -18, -21, -21, -19, -18, -18, -17},
        {0, -18, -16, -18, -18, -20, -28, -44},
    },
    {
        {},{},{},{},{},{},{},{},
        {},
        {0, 22, 26, 27, 30, 33, 40, 59},
        {0, 88, 92, 95, 97, 93, 88, 101},
        {0, 119, 120, 120, 120, 120, 116, 115},
        {0, 133, 135, 137, 140, 139, 136, 142},
        {0, 301, 319, 312, 306, 296, 285, 280},
        {0, 280, 278, 274, 270, 264, 253, 233},
        {},
        {},
        {0, -22, -26, -27, -30, -33, -40, -59},
        {0, -88, -92, -95, -97, -93, -88, -101},
        {0, -119, -120, -120, -120, -120, -116, -115},
        {0, -133, -135, -137, -140, -139, -136, -142},
        {0, -301, -319, -312, -306, -296, -285, -280},
        {0, -280, -278, -274, -270, -264, -253, -233},
    }
};
int16_t DOUBLED_MG[] = {8, -7, 16, 18, 19, 16, -1, 8};
int16_t DOUBLED_EG[] = {32, 20, 13, 8, 8, 13, 18, 31};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 5};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 23
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 30
#define ROOK_OPEN_EG 10
#define ROOK_SEMIOPEN_MG 17
#define ROOK_SEMIOPEN_EG 17

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
    unpack_full(0, PAWN, "        >UZ<46/)@PH?99-.@GRPIB21FO_[UM?;QZtqi_LB: i~tm5d        ", 1.016, -1); // average: 31
    unpack_full(1, PAWN, "        #'.531,-! ,--)(%$)(''*-),//,/1425F;AA>@;b@j~xrmy        ", 1.338, 61); // average: 86
    unpack_full(0, PASSED_PAWN, "        /=53.,+2.0&& !,1..$ $&68-70/25E?5*82<FNUTH<DXZ~W        ", 1.109, -15); // average: 5
    unpack_full(1, PASSED_PAWN, "        $$ !$$* (-'&$&)$6:40+,00DF=6027;URL;5:DJ^~X506E>        ", 2.131, -7); // average: 29
    unpack_smol(0, KING, "VDGg: #9UTMD~{aN", 1.043, -52); // average: -4
    unpack_smol(1, KING, "&(% 9DA7YZ[VZdd_", 1.0, -34); // average: 5
    unpack_half(0, QUEEN, " ()--15133200+/-", 1.0, 533, 534, 536, 549); // average: 547
    unpack_half(1, QUEEN, "B7( 55,00AGB4INW", 1.0, 1013, 1068, 1005, 1068); // average: 1040
    unpack_half(0, ROOK, "68>E )21-845+*34", 1.0, 229, 262, 234, 273); // average: 248
    unpack_half(1, ROOK, " &'\"--+-%&*))0/,", 1.0, 486, 512, 481, 505); // average: 496
    unpack_half(0, BISHOP, "\"   ,52-,9750/9B", 1.0, 190, 196, 189, 203); // average: 205
    unpack_half(1, BISHOP, " /%,)3560;CE+>DH", 1.0, 250, 259, 251, 259); // average: 271
    unpack_half(0, KNIGHT, " $*.(,72,>=D:7GE", 1.0, 183, 202, 186, 206); // average: 203
    unpack_half(1, KNIGHT, " CUXM^clWjry_rz~", 1.223, 172, 181, 172, 183); // average: 250
    
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
