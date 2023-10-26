#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(25, 78)
#define PASSED_PAWN_OFFSET S(-16, -9)
int QUADRANTS[] = {
    S(217, 215), S(240, 219), S(224, 213), S(247, 217),
    S(238, 305), S(248, 312), S(240, 307), S(254, 310),
    S(292, 546), S(317, 567), S(300, 539), S(337, 555),
    S(588, 1084), S(588, 1131), S(590, 1087), S(607, 1139),
};
#define BISHOP_PAIR S(24, 47)
int DOUBLED_PAWN[] = {S(6, 23), S(-10, 20), S(8, 19), S(13, 14), S(14, 10), S(9, 21), S(-7, 20), S(7, 32)};
#define TEMPO S(10, 11)
#define ISOLATED_PAWN S(9, 7)
int PROTECTED_PAWN[] = {0, S(10, 10), S(13, 15)};
#define ROOK_OPEN S(33, 13)
#define ROOK_SEMIOPEN S(16, 17)
int PAWN_SHIELD[] = {S(7, -21), S(18, -35), S(20, -29), S(27, -25)};
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-11, 20)
#define PAWN_NEIGHBOR S(4, 9)

int get_data(int i) {
    auto DATA_LOW = ";NN2+/) ?I@853&\"BEPLI>.)LP`]TG=6_f(oigID;-n1!uUm3E:7/05861)&&&,9,7) ()<C,998;DMG! 7>CHVQOVIBQRjEkRQqK !:L600YdPg %-.*/?9.BBL::NO#% \"098./<:9-0<H/15; -3.*700'(-. ().128487533312%+7C<956 \"146-+)%+)**.1-240,/69:DT>KNFQGko~wp}$x.-& &)3,0>3/'0;2ORI@79BFkj[JDEQ]4)}VNcp#FB#[c_h}\"3/ 1NL1E]]I4AH0 Hch]ks{dy&-o&05 /'-%24959EF/BFJ#&)$&%&$ !$#%)(&%\"'' &,6%=NL0FS`";
    auto DATA_HIGH = "                                  !        !!                                                                                                                                                                                 !                                 !!     !!!!                               !! !!!                                                ";
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
