#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_MATERIAL S(23, 70)
#define PASSED_PAWN_OFFSET S(-15, -11)
int QUADRANTS[] = {
    S(199, 189), S(214, 193), S(207, 190), S(220, 191),
    S(212, 259), S(212, 264), S(213, 261), S(216, 262),
    S(267, 474), S(280, 493), S(274, 469), S(300, 482),
    S(437, 774), S(423, 824), S(436, 805), S(443, 837),
};
#define BISHOP_PAIR S(29, 35)
int DOUBLED_PAWN[] = {S(2, 18), S(-11, 15), S(6, 15), S(11, 11), S(12, 7), S(7, 16), S(-10, 14), S(2, 26)};
#define TEMPO S(10, 10)
#define ISOLATED_PAWN S(9, 7)
int PROTECTED_PAWN[] = {0, S(6, 6), S(5, 6)};
#define ROOK_OPEN S(25, 2)
#define ROOK_SEMIOPEN S(11, 9)
int PAWN_SHIELD[] = {S(5, -10), S(11, -23), S(13, -19), S(20, -14)};
#define KING_OPEN S(-42, -3)
#define KING_SEMIOPEN S(-9, 16)
int MOBILITY[] = {S(4, 7), S(1, 5), S(4, 5), S(2, 4), S(1, 16), S(-4, 5)};

int get_data(int i) {
    auto DATA_LOW = "9NO732+ CMHB>;,(AFTPMB/*GN\\\\SF;1V_{gb_C;5*_zhjSV.=54,,051-%%$#*6,6' ((;A,6748AKE%\"7<AFUPPSLI\\Q^P\\GEbI  5SE:0u(kp )/2)1@?6FJR<>PO\"$ #(2/(+84.('+9/17> -42)510$&+* ),./15566720.-. '5?63-0\"%255.,+&-+,-/3/120-1689=N<HKCKAbio_^glf0.& %)5,3?4/)0;3QSJ@7:CGlm^LFHT`8/#[Shv(KF1uvv .\"1. 2JH4CUVH3;B1 @U[Q[Y_R]_d]fhl +**$&(*..01(01-\"$' (''&##&%(*)';10FA)0/<>1(?6, ";
    auto DATA_HIGH = "                                                                                                             !                                                                                                                                                  !!!    !!!!   !!                                                                                ";
    return DATA_LOW[i] + 95 * DATA_HIGH[i] +
        0x10000 * (DATA_LOW[i+176] + 95 * DATA_HIGH[i+176])
        - S(3072, 3072);
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
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            PST[BLACK | KING][70-10*rank+file] = -(
                PST[WHITE | KING][10*rank+file] = get_data(rank/2*4+file/2+96)
            );

            if (rank > 0 && rank < 7) {
                PST[WHITE_PAWN][10*rank+file] = PST[BLACK_PAWN][70-10*rank+file] =
                    get_data(rank*8+file-8) + PAWN_MATERIAL;

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + PASSED_PAWN_OFFSET;
            }

            for (int piece = KNIGHT; piece <= QUEEN; piece++) {
                PST[BLACK | piece][70-10*rank+file] = -(
                    PST[WHITE | piece][10*rank+file] = get_data(
                        (rank & 4 ? rank ^ 7 : rank)*4 + (file & 4 ? file ^ 7 : file) + piece*16+80
                    ) + QUADRANTS[piece*4-8+rank/4+file/4*2]
                );
            }
        }
    }
    
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
