#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(23, 85)
#define PASSED_PAWN_OFFSET S(-18, -7)
int QUADRANTS[] = {
    S(216, 233), S(234, 237), S(224, 231), S(240, 236),
    S(236, 301), S(238, 305), S(237, 302), S(242, 304),
    S(290, 555), S(307, 573), S(298, 547), S(328, 561),
    S(607, 1087), S(599, 1123), S(610, 1088), S(620, 1135),
};
#define BISHOP_PAIR S(23, 46)
int32_t DOUBLED_PAWN[] = {S(5, 25), S(-10, 22), S(6, 24), S(12, 16), S(10, 15), S(4, 28), S(-10, 24), S(4, 35)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(9, 9)
int PROTECTED_PAWN[] = {0, S(9, 9), S(11, 11)};
#define ROOK_OPEN S(25, 3)
#define ROOK_SEMIOPEN S(10, 13)
int PAWN_SHIELD[] = {S(11, -13), S(15, -26), S(14, -18), S(20, -10)};
int MOBILITY[] = {0, S(3, 2), S(1, 0), S(4, 5), S(2, 3), S(2, 4), S(-4, 5)};
int KING_FILES_OPEN[] = {S(-13, -4), S(-50, -2), S(-21, -1)};
int KING_FILES_SEMIOPEN[] = {S(-10, 13), S(-16, 24), S(-9, 22)};

int get_data(int i) {
    auto DATA_LOW = ":MI621*\"BIA>:8)(@AKKH?-*GJTXOD92V\\vd_]B>B \\&~b=n3E<7-.4854))%%+9/9+#,)=C-9;8<DMG# :>EIVRBWM?FZy=aLKgF  4NB6,j*mm )02*1@?4FHQ<=PO\"$ $(30)*84/'',:/28? -43*612#%+* ),.--22420-+(&&\")9>65/3 \"224,()&+,)+-0-223*/67:DSBIMGNFwy569BG<.+! !$.&0=1,\"*5,OSH?15=Bln\\MBDP[80!ZNds%A?uB=?G\\\"40 5QM5I^]L7@E1 Lej\\ktycy$,p'16 -**$**,0035*553\"#' '&&%\"\"%$'*)'% \"*($).,<E?6FMR";
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
