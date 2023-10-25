#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(124, 283), S(140, 297), S(129, 284), S(145, 301),
    S(189, 357), S(193, 367), S(189, 359), S(200, 369),
    S(214, 619), S(233, 657), S(220, 612), S(249, 648),
    S(467, 1183), S(460, 1246), S(470, 1188), S(476, 1258),
};

#define BISHOP_PAIR S(13, 61)
int32_t DOUBLED_PAWN[] = {S(8, 35), S(-9, 29), S(13, 25), S(16, 13), S(18, 8), S(10, 19), S(-4, 20), S(5, 44)};
#define TEMPO S(5, -1)
#define ISOLATED_PAWN S(6, 9)
int32_t PROTECTED_PAWN[] = {0, S(6, 10), S(7, 7)};
#define ROOK_OPEN S(25, 17)
#define ROOK_SEMIOPEN S(15, 15)
int32_t PAWN_SHIELD[] = {S(15, -37), S(20, -42), S(21, -27), S(26, -18)};
#define KING_OPEN S(-31, -5)
#define KING_SEMIOPEN S(-8, 11)

int get_data(int i) {
    auto DATA_LOW = "4GJ,)1+ 8G<626*(8>JIE>/*?JUSPD=2QNfa]TC;%&]%Y3*V/:01/-,8.-%#$\".50&\"!))<<-5334:IC,+99HFQRgDP7`&fOhNSpR \"FKI>&GvI# PSWV[f_Xliqgcro-0\"!384..<75308>027@ -23)516+)13!&\"&-,1-,.++,'+')19C?=46\"!525-+)*5,*+.416:<33;<=D[JQLLUK@Sr*u2Bo)#!!+&3 060+$'/+QPM=36>Drl]GEES_5 xSLal!j[~OZE^9%8- 9[T2m# hc*.c!?[aI`dr[t!-i\"08!0%,*3:96BIN'HSW!+-&,+,-$(.*#00-6)*\" +)32BSO3MVk";
    auto DATA_HIGH = "                                           !                                                 !              !  !                                                                                                           ! !!                                 !!     !!!     !         !!  !!           !! !!!                                                ";
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
                    get_data(rank*8+file-8) + S(23, 95);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-19, -4);
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
