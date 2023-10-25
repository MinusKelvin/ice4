#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(168, 140), S(191, 143), S(175, 138), S(200, 142),
    S(260, 176), S(264, 181), S(260, 175), S(274, 181),
    S(320, 333), S(358, 346), S(328, 328), S(377, 337),
    S(626, 643), S(629, 682), S(631, 641), S(657, 680),
};

#define BISHOP_PAIR S(22, 39)
int32_t DOUBLED_PAWN[] = {S(14, 16), S(-8, 17), S(19, 10), S(18, 7), S(24, 0), S(14, 11), S(-3, 12), S(12, 22)};
#define TEMPO S(6, -2)
#define ISOLATED_PAWN S(8, 7)
int32_t PROTECTED_PAWN[] = {0, S(8, 4), S(9, 2)};
#define ROOK_OPEN S(36, 0)
#define ROOK_SEMIOPEN S(21, 4)
int32_t PAWN_SHIELD[] = {S(14, -17), S(24, -33), S(25, -21), S(30, -18)};
#define KING_OPEN S(-41, 3)
#define KING_SEMIOPEN S(-12, 14)

int get_data(int i) {
    auto DATA_LOW = "9QW/*5-!>PD;5:+*>EURND1-IUc_[ME:]\\wol]NE'4w0cI3j5D8:964<64-+&#37=8) (+HC>GA9;DVQI@QHU_gi4aT[(9.vaCIkC\" <TYVG0oUa!gkrpv(}r/+9(#86+3$!9B=45HC?9:EO36?H /:8+:7;))56!'!'..51-210,)0.-.3DB<6:$!0.0+,+&.'$%+2-+/.),2332?<>:=@9,DD#ou{I/.-*.*5(7?793662EIIH@=;>USNHE?CId`VICEMY*)w!%$4^*5- 9HC/DE?86F?*%!7:+:>I7CPS<RW[ *!*#&-/+/6:%7;: '%!*(%'%$'&'-,*-(,# &'..8EG1IKV";
    auto DATA_HIGH = "                                           !                                            !   !!!             !         !  !!!!!!!                                                                                           !                                            !!                                                                                      ";
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
                    get_data(rank*8+file-8) + S(36, 54);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-22, -12);
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
