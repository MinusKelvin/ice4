#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(25, 76)
#define PASSED_PAWN_OFFSET S(-17, -9)
int QUADRANTS[] = {
    S(204, 188), S(227, 194), S(211, 188), S(234, 192),
    S(222, 272), S(233, 280), S(224, 275), S(238, 278),
    S(278, 483), S(302, 506), S(286, 478), S(321, 495),
    S(511, 932), S(506, 1007), S(510, 954), S(526, 1012),
};
#define BISHOP_PAIR S(28, 40)
int DOUBLED_PAWN[] = {S(5, 22), S(-10, 20), S(9, 19), S(13, 14), S(13, 11), S(10, 21), S(-8, 20), S(5, 31)};
#define TEMPO S(10, 10)
#define ISOLATED_PAWN S(9, 8)
int PROTECTED_PAWN[] = {0, S(8, 7), S(9, 7)};
#define ROOK_OPEN S(32, 15)
#define ROOK_SEMIOPEN S(15, 17)
int PAWN_SHIELD[] = {S(8, -20), S(16, -34), S(19, -27), S(27, -20)};
#define KING_OPEN S(-42, -1)
#define KING_SEMIOPEN S(-10, 19)
#define DATA_LOW ";OO4-1) >I@:65'$@DPLI>-)JN_\\SH<5\\c&mhgHC7-f ooX^4B76..4960(%%%,:.8) ();C.:88<CMG%\"6>CFVRUXQP`WdSgONnJ \":I4-.HI8\\ &-/*/?9/BBL::NN\"$ \"/88./<99,0;G.059 ,2-*6/.&',, '(-237496523423%*5@8735 !/23+))$*(((,/,01.)-468?O;EHBLC[_eYTad^.,' &)3,0>3/(0;2NPH@78AEggXICCNZ,#wTL^j{FA-mrny*!2/ /LI0DZ[H7EK3 D\\`Vdkr]p|$g|'+ .'+$12737CD.@DG$&($%%%$ !$#$((%'&,2 *5<%CQR1FVb"
#define DATA_HIGH "                                  !        !                                                                                                                                                                                                                    !!      !!!    !                           !  !!                                                "

int get_data(int i) {
    auto data_low = DATA_LOW;
    auto data_high = DATA_HIGH;
    return data_low[i] + 95 * data_high[i] +
        0x10000 * (data_low[i+176] + 95 * data_high[i+176])
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
