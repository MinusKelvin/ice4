#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

#define PAWN_OFFSET S(26, 43)
#define PASSED_PAWN_OFFSET S(-20, -8)
int QUADRANTS[] = {
    S(264, 221), S(276, 225), S(271, 219), S(277, 225),
    S(246, 290), S(247, 295), S(247, 291), S(248, 296),
    S(324, 538), S(336, 558), S(332, 530), S(348, 549),
    S(621, 1195), S(607, 1213), S(624, 1190), S(620, 1205),
};
#define BISHOP_PAIR S(23, 46)
int32_t DOUBLED_PAWN[] = {S(5, 25), S(-10, 21), S(8, 21), S(13, 15), S(13, 11), S(9, 21), S(-8, 18), S(5, 29)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(9, 8)
int PROTECTED_PAWN[] = {0, S(9, 9), S(11, 11)};
#define ROOK_OPEN S(26, 4)
#define ROOK_SEMIOPEN S(10, 14)
int PAWN_SHIELD[] = {S(-21, 9), S(-17, 6), S(-16, 15), S(-8, 21)};
#define KING_OPEN S(-44, 0)
#define KING_SEMIOPEN S(-10, 15)
int MOBILITY[] = {0, S(3, 2), S(1, 0), S(4, 5), S(2, 2), S(2, 4), S(-4, 3)};
int TROPISM[] = {0, S(0, 7), S(-7, 2), S(-2, 2), S(-7, 3), S(-7, -19), S(0, 0)};

int get_data(int i) {
    auto DATA_LOW = "9LL52/( AID=:6'%?APKH=+(FJWWOB71V\\yd_]B=?!^\"y`<i5D<8/08<62'(&)0=08+#,-AH/7;9=FPI& :@EJYTI[RDM_ FXFC^? $6?<3 Ry`W )02#)87'7:A,+;:\"$ #&0-'&40*##&58:@G\"-42(2++  \" /8;>44;;531.+&!  (7@7836$'5;<531-6688;;7<AA>@EDEQdU_bX]R25MTSTZO2.$ &'.$5B7.(+3(UZMB67=>rsaNGFPY:4$]Sep!47p<;>?P!)' */,'@>>A2*0) Mfl`px|i *2v.9> -)+%,-.3479-9:8 \"& **+*&(,,,/103&%05' %.12(5977";
    auto DATA_HIGH = "                                           !                                                  !                                                                                                                         !!!!!!!!                                !!!    !!!                               !!! !!!                                                ";
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
