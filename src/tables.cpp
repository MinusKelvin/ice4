#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(28, 85)
#define PASSED_PAWN_OFFSET S(-18, -9)
int QUADRANTS[] = {
    S(216, 233), S(233, 237), S(223, 231), S(239, 236),
    S(236, 300), S(237, 305), S(237, 301), S(241, 304),
    S(289, 554), S(305, 572), S(297, 546), S(326, 560),
    S(606, 1087), S(598, 1123), S(609, 1087), S(618, 1134),
};
#define BISHOP_PAIR S(23, 46)
int32_t DOUBLED_PAWN[] = {S(8, 24), S(-8, 23), S(10, 22), S(15, 16), S(16, 12), S(11, 24), S(-5, 22), S(8, 33)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(9, 9)
int PROTECTED_PAWN[] = {0, S(9, 9), S(11, 12)};
#define ROOK_OPEN S(25, 3)
#define ROOK_SEMIOPEN S(10, 12)
int PAWN_SHIELD[] = {S(5, -10), S(12, -24), S(14, -18), S(21, -11)};
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 19)
int MOBILITY[] = {0, S(3, 2), S(1, 0), S(4, 5), S(2, 3), S(2, 4), S(-4, 5)};
#define PAWN_SEMIOPEN S(-3, -5)

int get_data(int i) {
    auto DATA_LOW = ";NL620( CJE>;7'&ACQLI>+(IMZYPC81X^{f`]A<A#`&}a<k2D<7-/4852''$&,9/8)\"**>D-8:8;DNH# :>DJWRCWM?G[y>cMKiI !6N=4/g ho )/2*1?>4FHQ<=OO\"$ $(30)*84/((,:/27? ,42*612#&+* )+.--21420-+(&%\")7@6503 \"/33+))&*)**-0.220+.57;CR?IMFOFwx378AF=/,$ &(3+2>3-'.91QTJ@69BGno^MFGT`:1$[Rgv)BAvBBCL`\"2. 3MJ5GZZK6=D1 Mfk]luycy%,p'27 -**$**,0035*553!#' '&&%\"\"%$'*)'% \"+($).+;D?6FLR";
    auto DATA_HIGH = "                                           !                                                                 !                                                                                                            !!!!!!                                !!!    !!!                                !! !!!                                                ";
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
                    get_data(rank*8+file-8) + PAWN_OFFSET;

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
