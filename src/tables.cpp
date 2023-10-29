#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(49, 161)
#define PASSED_PAWN_OFFSET S(-37, -19)
int QUADRANTS[] = {
    S(431, 466), S(465, 475), S(446, 462), S(478, 471),
    S(471, 600), S(474, 610), S(473, 603), S(482, 608),
    S(578, 1109), S(610, 1145), S(593, 1093), S(653, 1120),
    S(1213, 2173), S(1197, 2245), S(1219, 2173), S(1238, 2267),
};
#define BISHOP_PAIR S(46, 93)
int32_t DOUBLED_PAWN[] = {S(11, 43), S(-19, 39), S(16, 39), S(27, 26), S(28, 20), S(18, 43), S(-14, 37), S(11, 62)};
#define TEMPO S(21, 23)
#define ISOLATED_PAWN S(19, 17)
int PROTECTED_PAWN[] = {0, S(18, 17), S(21, 23)};
#define ROOK_OPEN S(51, 7)
#define ROOK_SEMIOPEN S(21, 24)
int PAWN_SHIELD[] = {S(9, -21), S(23, -48), S(27, -36), S(42, -23)};
#define KING_OPEN S(-88, -4)
#define KING_SEMIOPEN S(-21, 37)
int MOBILITY[] = {0, S(5, 3), S(3, -1), S(7, 10), S(5, 5), S(3, 8), S(-9, 11)};

int get_data(int i) {
    auto DATA_LOW = "U{yKC?0 fuj[UN.,bf#xr\\50qy43\"eOA1=xLA;bWa#=,{AWVGjYO<?IRLF/0*-9T?Q4%56^j;QUPXj~p' T^ju0'h1~an9v]HzwSr !L}YI>N~Q` 1>D3B^\\Hmp#XY ~%) '0E?14QI=//7U?DO^ 9GE4MCC'+65 17<;;DCGDA961+*$2O`MJ?F #?GG723+5245:A;CD@6=KOUf&^szm~msrIQSdm]?8( ,1G7D\\G;/<SC#*t`MRdn]_=zmo)AUC'7&Pm2bblbadw@$C< GztIn45vL[hA yLW;YjsGs)8a-CM :44(448@@GJ4JJF#'. .,-*#$)(.41.+ $51(1;7Wi^Lly%";
    auto DATA_HIGH = "                  !       !!!   !!!!!!    !\"!! !                                      !! !   !! !  !        !!!!           !  !                                                                                  !      !!\"\"\"\"\"\"                !!      !!!   !!\"\"\"!!!!\"\"\"!    !         !!       !!!!!!!!\"\"!\"\"\"                                               !";
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
