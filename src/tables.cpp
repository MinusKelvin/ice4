#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int STARTS[] = {0,0,8,4,0,0,0};
int LIMITS[] = {0,0,1,8,8,8,1};
int ENDS[] = {0,0,16,8,4,8,8};
int SLIDER[] = {ROOK, ROOK, ROOK, ROOK, BISHOP, BISHOP, BISHOP, BISHOP, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT};
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};

#define PAWN_OFFSET S(24, 81)
#define PASSED_PAWN_OFFSET S(-18, -10)
int QUADRANTS[] = {
    S(215, 233), S(233, 238), S(223, 230), S(239, 237),
    S(236, 300), S(238, 306), S(237, 301), S(242, 304),
    S(289, 553), S(306, 572), S(297, 544), S(328, 559),
    S(607, 1086), S(599, 1123), S(610, 1086), S(620, 1133),
};
#define BISHOP_PAIR S(23, 47)
int DOUBLED_PAWN[] = {S(5, 22), S(-10, 20), S(8, 19), S(14, 13), S(14, 10), S(9, 21), S(-7, 18), S(6, 31)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(9, 9)
int PROTECTED_PAWN[] = {0, S(9, 8), S(11, 11)};
#define ROOK_OPEN S(26, 3)
#define ROOK_SEMIOPEN S(10, 12)
int PAWN_SHIELD[] = {S(5, -6), S(11, -19), S(13, -13), S(21, -8)};
#define KING_OPEN S(-41, 3)
#define KING_SEMIOPEN S(-9, 18)
int MOBILITY[] = {0, S(3, 1), S(1, 0), S(4, 5), S(2, 3), S(2, 4), S(-3, 8)};
int VQM[] = {S(8, 13), S(7, 1), S(-1, 4), S(-23, 8), S(-39, -1), S(-31, -18), S(-29, -34)};

int get_data(int i) {
    auto DATA_LOW = ";NL610( CJE=;7'&ACQLI>+(IMZZQC81X_|g`]A<C%`' a=k3D;6./4952''%&,:/8*\"**>D-9:8;DNH# :>DIWRCWN@F[z>_JHeH !6Q<41f#jk (/1*1?>4FHP<<OO\"$ #)30)*94.((,;/17= ,32*722#&,+ )+...22421-+)&&\")7?5403 !/32+))%*))*-1.220+.68;DT@KNGPGy{69:BH>0,# &(4,2>2-'/:2RUJ@8:CHoo^NHIUa;2$\\Tiw)A@uCCDL`\"2/ 4RO5FaaJ4;B. Mej]ltycy%,p'27 -)*$*)+0034*552#%' (''&#\"%$(+)'& !()$(.+;D?6ELR";
    auto DATA_HIGH = "                                           !!                                                                !                                                                                                            !!!!!!                                !!!    !!!                                !! !!!                                                ";
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
