#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(19, 65)
#define PASSED_PAWN_OFFSET S(-15, -8)
int QUADRANTS[] = {
    S(173, 184), S(187, 188), S(179, 183), S(192, 186),
    S(189, 240), S(190, 244), S(190, 240), S(193, 243),
    S(231, 442), S(245, 456), S(238, 435), S(262, 446),
    S(487, 856), S(480, 886), S(489, 856), S(496, 896),
};
#define BISHOP_PAIR S(19, 37)
int32_t DOUBLED_PAWN[] = {S(4, 18), S(-8, 16), S(6, 16), S(11, 12), S(11, 9), S(7, 18), S(-6, 16), S(5, 25)};
#define TEMPO S(9, 9)
#define ISOLATED_PAWN S(8, 7)
int PROTECTED_PAWN[] = {0, S(7, 7), S(9, 9)};
#define ROOK_OPEN S(21, 2)
#define ROOK_SEMIOPEN S(8, 10)
int PAWN_SHIELD[] = {S(3, -7), S(9, -19), S(11, -15), S(17, -9)};
#define KING_OPEN S(-36, -2)
#define KING_SEMIOPEN S(-9, 16)
int MOBILITY[] = {0, S(2, 1), S(1, 0), S(3, 4), S(2, 2), S(1, 3), S(-4, 4)};

int get_data(int i) {
    auto DATA_LOW = "6ED2/-& =B>863&%;<HDA8)'ADONG<3-NRjYSQ;6;\"PngQ6Z0>73,-1520&'$&+6,4(\"))9>+4647>FA# 59?BLJ>MJ>CTi;VDC[A !2E4-*[lX^ ',/(.981?AH77GF!# #&/-'(41,&&*6,.39 *0/(2..\"$)( ')+++/.0/-*)'$$\"(4;32.0 \"-00*(($)'))+.+//-*,246=J:BF@G@kk%*+110-*# %'0*/91+',5/IKC;35<@_`RE@AJT\"zoQJ[es$%^446@J\".+ 0DA0?MMB/49* CX\\Q]ehVhqw`r{  )''#''),,/0'00.!#& %$$#!!##&('%$ \"))%)-,9@;4AGK";
    auto DATA_HIGH = "                                                                                                                                                                                                                          !!!!!!                                !       !!                                     !                                                ";
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
