#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(162, 222), S(191, 224), S(171, 219), S(202, 219),
    S(258, 272), S(268, 279), S(261, 272), S(276, 276),
    S(321, 499), S(353, 517), S(332, 491), S(379, 503),
    S(689, 954), S(691, 991), S(694, 952), S(720, 996),
};

#define BISHOP_PAIR S(25, 56)
int32_t DOUBLED_PAWN[] = {S(7, 21), S(-9, 17), S(12, 20), S(12, 17), S(16, 9), S(11, 23), S(-6, 18), S(10, 30)};
#define TEMPO S(13, 10)
#define ISOLATED_PAWN S(11, 9)
int32_t PROTECTED_PAWN[] = {0, S(10, 8), S(12, 7)};
#define ROOK_OPEN S(41, 9)
#define ROOK_SEMIOPEN S(20, 12)
int32_t PAWN_SHIELD[] = {S(9, -20), S(21, -38), S(24, -30), S(33, -26)};
#define KING_OPEN S(-54, 3)
#define KING_SEMIOPEN S(-14, 19)

int get_data(int i) {
    auto DATA_LOW = "D\\[4*6.!GTE?8:*&GJXWQD2+SZlgZNE9kl4xqlMI7;vH2\\Gp7J;?66:B980)*(4B45, ,-FJ3A<<DOUS).@IRXf]l[^K\\% ]{ZY\"S$ BO/)1$l=` hnopu+#t10@'#>=)9$!8DE48FCC48EU138> .51+;32'(-,!$ *41828630/012&+9MC>7;!$58;0--%.**,/40131-289;?P@ISISGb!\"C6JA,4,)!+(5,8?44)1:4RQKG;;@Ghe[MEBPZ,\"wVG^iz5zl&+#?X%71!6RN4D[ZE)CI+ )DJ<JQYFTcgLglr *!-\"*-4/4>B+>CB$(+&)(((#!$$&**)&,60 +2<+CUS8PVd";
    auto DATA_HIGH = "                                  !        !!                                                !!    !        !         !! !!!!!!!                                                                                         !!!!!!!                                !!      !                                                                                       ";
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
                    get_data(rank*8+file-8) + S(31, 74);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-24, -12);
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
