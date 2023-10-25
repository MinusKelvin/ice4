#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(218, 227), S(242, 231), S(225, 225), S(248, 229),
    S(240, 319), S(250, 324), S(242, 319), S(255, 323),
    S(292, 571), S(318, 591), S(300, 562), S(339, 579),
    S(650, 1108), S(651, 1149), S(653, 1107), S(671, 1158),
};

#define BISHOP_PAIR S(24, 48)
int32_t DOUBLED_PAWN[] = {S(6, 23), S(-10, 20), S(10, 20), S(14, 15), S(15, 11), S(11, 23), S(-7, 20), S(7, 32)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(10, 9)
int32_t PROTECTED_PAWN[] = {0, S(8, 7), S(9, 8)};
#define ROOK_OPEN S(33, 12)
#define ROOK_SEMIOPEN S(16, 17)
int32_t PAWN_SHIELD[] = {S(8, -22), S(17, -36), S(19, -29), S(27, -21)};
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-11, 21)

int get_data(int i) {
    auto DATA_LOW = "=QR5.2) AKC;76($CFRNJ?.*LPa^UI=6_e)pjhIDD*h3(iDs7I=9128<:5+(((.=0;,#+,?G/<;;?FPJ#!9@GKYUH[SCLb~ClSRsL \"<N711]mYm!&./*0@:/CCM;:OO$&!#099/0=::-1<I026;!-3/+810'(-.!)*/149598643322$+7C;:56 \"157.+*%+**+/1.231,/79;DT?KNGQGxw489CE>0,&!')4-1?3/'/;3QTIA89CHmm\\LFFS_8-!YPds&??r??=K]#51!2QN3G__K5AI0 Iej^mu}f{(/q)38 0(.&35:6:FH0DHK#'*&'''&!\"%%&*)'%!%$ %*5%<MK/FS_";
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
                    get_data(rank*8+file-8) + S(26, 83);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-20, -10);
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
