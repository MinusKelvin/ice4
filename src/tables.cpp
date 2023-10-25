#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(138, 201), S(160, 212), S(145, 200), S(168, 211),
    S(223, 249), S(225, 259), S(223, 248), S(235, 257),
    S(259, 446), S(297, 465), S(266, 439), S(313, 456),
    S(316, 1036), S(311, 1091), S(320, 1036), S(332, 1097),
};

#define BISHOP_PAIR S(23, 44)
int32_t DOUBLED_PAWN[] = {S(8, 34), S(-8, 31), S(17, 22), S(16, 16), S(20, 10), S(12, 21), S(-6, 23), S(3, 43)};
#define TEMPO S(46, 55)
#define ISOLATED_PAWN S(7, 6)
int32_t PROTECTED_PAWN[] = {0, S(9, 7), S(10, 3)};
#define ROOK_OPEN S(37, 3)
#define ROOK_SEMIOPEN S(21, 8)
int32_t PAWN_SHIELD[] = {S(13, -18), S(23, -37), S(26, -25), S(33, -18)};
#define KING_OPEN S(-39, -2)
#define KING_SEMIOPEN S(-10, 16)

int get_data(int i) {
    auto DATA_LOW = "7RV-(50 <QB;5;.+=EUUPF5-GXeb^QI:^axsogUEB<#G|LN$5=6?;63?6/0,+)6>:4( +.GG<A=7;FYUC9BDUZdlrJG6g4kUcCHmJ !?BEQ7,8Cu ehpls%zq-*6'#74#7%!<C?67JDA==FO59AL 2::-=8<.-78!405??FA?A?@>9><+05DB<8:#!2.0*-*(2(\"$,5/034+0678<NHOPHMDBl|<@\\;~KJE@DERCV_UUNPRPoopj_\\_e'$zpiclw?2+l`oz-iN'I9 a~(9/!;QK1fj_WDl`@ (:;+>@L9KW_@Xaf$&!'#'0/-5;@!9BB (&\".+*-$&++$021@)&#!*$0-CRN3RZh";
    auto DATA_HIGH = "                                          !!   !                                             !              \"!!!      !  !!!!!!!                                                                                           !!!!                         !!      !!!    !!!!                                                                                     ";
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
                    get_data(rank*8+file-8) + S(30, 79);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-27, -50);
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
