#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(26, 83)
#define PASSED_PAWN_OFFSET S(-19, -9)
int QUADRANTS[] = {
    S(219, 227), S(242, 231), S(225, 225), S(249, 229),
    S(241, 319), S(251, 325), S(242, 320), S(256, 324),
    S(293, 572), S(318, 592), S(301, 563), S(339, 580),
    S(651, 1108), S(653, 1149), S(654, 1106), S(673, 1158),
};
#define BISHOP_PAIR S(24, 48)
int32_t DOUBLED_PAWN[] = {S(6, 23), S(-10, 20), S(10, 20), S(14, 15), S(15, 11), S(11, 23), S(-7, 20), S(7, 32)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(10, 9)
int32_t PROTECTED_PAWN[] = {0, S(8, 7), S(9, 8)};
#define ROOK_OPEN S(33, 13)
#define ROOK_SEMIOPEN S(15, 17)
int32_t PAWN_SHIELD[] = {S(8, -22), S(17, -36), S(19, -29), S(27, -21)};
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-11, 21)
#define DOUBLED_ROOKS S(14, -5)

int get_data(int i) {
    auto DATA_LOW = "<QQ5.2) @KB;75($BERMJ?.*LPa^UH<5^e)pjgIDC*g2(hDr7H=9017;:4*''(.<0:+\"*+?F.;;;>FPJ# 9@FJXTHZSBLa}AlSRsL \"<M611]lYm %-/*0?9/CBL::OO#% \"/99./=99-0;H/15; ,2-*70.&',, ().038487532312$*7C;956 \"156.**%+***.1-131,/68:CT?JNGQGwv478CE=/,& &(3,1>3/&/;2PSI@78BGml[KEER_7- XOdr&?>q>>=J]\"40 2PM2F^_J4AH/ Idi^mu|ez(/q(37 0'-&3596:FG0CGK\"&)$&&&% !%$%)(&%!%$ %*5%<MK/FS_";
    auto DATA_HIGH = "                                  !        !!                                                                                                                                                                             !!!!!!                                !!!    !!!                                !! !!!                                                ";
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
