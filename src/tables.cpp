int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {9, -7, 17, 19, 20, 17, -1, 8};
int16_t DOUBLED_EG[] = {28, 19, 12, 5, 6, 12, 18, 33};
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
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 17
#define ROOK_SEMIOPEN_EG 14

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
    unpack_full(0, PAWN, "        CY_A;=6/FVPGA@45FNZXQI98NWfc]THBZb|wpeRIA f~sk4h        ", 1.048, 16); // average: 54
    unpack_full(1, PAWN, "        ')0632,/# +++''%&(&%%(,)-/-*-/227D9>>=?;`Gl~ytqx        ", 1.335, 90); // average: 115
    unpack_full(0, PASSED_PAWN, "        .:32.,+1-0%& \",0.-%!%(57.61036D=6+94=FMRSLCH[]~V        ", 1.25, -15); // average: 7
    unpack_full(1, PASSED_PAWN, "        $$ !$&-$(-'&&(,)6:30,.34DF=514:@VTM;6<GOb~X507FD        ", 2.056, -8); // average: 28
    unpack_full(0, KING, "<=8/62CG<6/%'19@*-!!\"$,*(,(($') -734-10':50842-1G7997.$6~amb[LUP", 2.438, -57); // average: -2
    unpack_full(1, KING, "6<>:5<9,@HNTSNE=JQY\\\\WPJOY_aa^WPXdggifcV^tunots`S{xrrv~T YY]]Z\\(", 1.72, -92); // average: 4
    unpack_half(0, QUEEN, " (),.15145322-1/", 1.0, 639, 639, 642, 654); // average: 654
    unpack_half(1, QUEEN, "9.$ ,/,5.COL4NXd", 1.0, 1273, 1330, 1268, 1336); // average: 1300
    unpack_half(0, ROOK, "139> (20*501('/0", 1.0, 279, 311, 284, 323); // average: 294
    unpack_half(1, ROOK, " &%!('&($$(&'-,)", 1.0, 631, 656, 624, 648); // average: 637
    unpack_half(0, BISHOP, "#!! .73..<9611;D", 1.0, 237, 242, 235, 249); // average: 253
    unpack_half(1, BISHOP, " 0&.*2772=DG-@FI", 1.0, 369, 379, 369, 378); // average: 391
    unpack_half(0, KNIGHT, " $*.(,94,>>F;7HG", 1.0, 221, 240, 225, 246); // average: 242
    unpack_half(1, KNIGHT, " CUYL]bjVirx_rz~", 1.238, 265, 275, 265, 276); // average: 344
    
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
