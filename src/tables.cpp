#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(168, 233), S(196, 235), S(176, 229), S(206, 230),
    S(265, 287), S(275, 294), S(267, 288), S(283, 291),
    S(330, 522), S(360, 541), S(341, 512), S(385, 526),
    S(702, 1006), S(702, 1042), S(707, 1004), S(727, 1053),
};

#define BISHOP_PAIR S(25, 57)
int32_t DOUBLED_PAWN[] = {S(10, 20), S(-9, 17), S(12, 21), S(14, 15), S(16, 10), S(11, 23), S(-7, 17), S(9, 31)};
#define TEMPO S(13, 11)
#define ISOLATED_PAWN S(11, 9)
int32_t PROTECTED_PAWN[] = {0, S(10, 8), S(12, 8)};
#define ROOK_OPEN S(40, 10)
#define ROOK_SEMIOPEN S(19, 13)
int32_t PAWN_SHIELD[] = {S(9, -21), S(19, -39), S(23, -31), S(32, -26)};
#define KING_OPEN S(-53, 2)
#define KING_SEMIOPEN S(-14, 19)

int get_data(int i) {
    auto DATA_LOW = "AYX2(4, ERC=79(%EGVUPC0+QXjeYMC8jk5wonLH'0y@(OCv8H9:658C961()(5@36-!,.DL0D;<EOVR'';JTSg_{m]Pg9)XtTS{R#!?M-'*\"|Zs!hnpqu*#u10?'$>=(8# 7CC37DAA37CS139?!062+=34*)..!$ )3071741..-0/%*8MB=6: \"48:/,,%-*)+.3/020-198;@Q=MVFRGd x4>SD::3.%/.=2=G99/6@;YXPLBAHOqmcSKHYb8,$XJjq%= x<+ CV%92 5RN3E]^G)AC&!+GN>PT]JXgmPjqw *!-\"*-4/4?B+=CC$),&+)()#!%$&+*)$'40!/4>+EXV:TYh";
    auto DATA_HIGH = "                                  !        !!                                                !!             !         !! !!!!!!!                                                                                         ! !!!!!                                !!!    !!!                                                                                      ";
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
                    get_data(rank*8+file-8) + S(34, 78);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-24, -17);
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
