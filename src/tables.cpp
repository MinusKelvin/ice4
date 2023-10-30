#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(38, 117)
#define PASSED_PAWN_OFFSET S(-24, -10)
int QUADRANTS[] = {
    S(330, 345), S(355, 353), S(340, 343), S(363, 350),
    S(360, 440), S(364, 446), S(362, 442), S(369, 444),
    S(446, 817), S(469, 844), S(456, 806), S(499, 826),
    S(878, 1642), S(866, 1694), S(883, 1645), S(897, 1705),
};
#define BISHOP_PAIR S(33, 67)
int32_t DOUBLED_PAWN[] = {S(8, 29), S(-14, 27), S(12, 26), S(19, 15), S(20, 12), S(13, 29), S(-9, 25), S(8, 46)};
#define TEMPO S(15, 19)
#define ISOLATED_PAWN S(14, 13)
int PROTECTED_PAWN[] = {0, S(13, 12), S(16, 16)};
#define ROOK_OPEN S(36, 5)
#define ROOK_SEMIOPEN S(16, 16)
int PAWN_SHIELD[] = {S(10, -21), S(19, -36), S(21, -27), S(31, -21)};
#define KING_OPEN S(-64, -3)
#define KING_SEMIOPEN S(-15, 25)
int MOBILITY[] = {0, S(4, 2), S(2, 0), S(5, 7), S(3, 4), S(2, 6), S(-6, 7)};

int get_data(int i) {
    auto DATA_LOW = "IhfB;:/\"VcZNJE.,RWkd_O3.]fxviVG;t J+%}TKM %aR/O18SE@12:A=9)(#%/B5B+ +,JT1CEAFUcZ$!HLQ^ujXv\\KXpFN\"_\\*[ !?bQH;!G)3 -8<0:NO>Z\\jLLgf%' %,<8-.D>5,*0F8<DO 3></B;;'+20 -1544<;=;931.)'\"*>K><39 !599.+,(.,+,152874,2<>DPdGY]S^S:7URTbmZ50% )+</7J82)2D7dk[N@BOV44z`UUl}xhRqe$@[)+Qfaes8!;8 <c_@ZxycGT`E d%.y.9@!?NY4Rah 622'/149:?A0AB@#&* -+,*$%)(+/.,* #/'!(/*ALE9NW_";
    auto DATA_HIGH = "                                 !!!!     !!!! !                                              ! !  !        !!!!                                                                                                        !!!!!!!!                        !!      !!!  !!!\"\"!    !                  !! !!!!!!!!!!!                                                ";
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
