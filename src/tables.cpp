#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(183, 155), S(204, 159), S(189, 153), S(212, 157),
    S(276, 189), S(281, 194), S(276, 188), S(291, 194),
    S(344, 355), S(380, 368), S(351, 349), S(397, 359),
    S(635, 726), S(635, 762), S(640, 722), S(657, 763),
};

#define BISHOP_PAIR S(21, 42)
int32_t DOUBLED_PAWN[] = {S(18, 15), S(-7, 16), S(19, 11), S(21, 4), S(24, 1), S(15, 11), S(-4, 8), S(11, 23)};
#define TEMPO S(6, -2)
#define ISOLATED_PAWN S(9, 8)
int32_t PROTECTED_PAWN[] = {0, S(8, 5), S(10, 3)};
#define ROOK_OPEN S(34, 1)
#define ROOK_SEMIOPEN S(21, 4)
int32_t PAWN_SHIELD[] = {S(12, -17), S(21, -33), S(23, -22), S(28, -18)};
#define KING_OPEN S(-37, 1)
#define KING_SEMIOPEN S(-11, 14)

int get_data(int i) {
    auto DATA_LOW = "6NT-'3+ ;MA949)*;ARQMC0,EQa^ZKC9\\ZwqjaPF&&p/O73t2<3491/:20+'$ 34:7+ '+CB:J>79DWNE:JCWXgi6q[Y=Q7nQ59[?\"!3\\eUINM>*!flrqt&|r/*7(#75+3$!9A<45FA>78DM37?I!1::+<7<,+68!(\"'.-50-1/.,'.,.-3DB=7;% 101,-,(.($&,3.,/.*-4454@9@>:?:$D>nr-xZDFA>?AL<LVKMHIKHZ][[UQRVkhc\\YTZ`yum[R_bqF7+C1 Hd-:3$;ID2FFB81>1  !68(<<G6ANS;PVZ!+#+$&./,06;&6;9 ($ *'$&%#'$&,*)1!&!\",(//=II4NN[";
    auto DATA_HIGH = "                                           !                                            !   !!!             !!!!      !  !!!!!!!                                                                                             !                                          !!!                                                                                     ";
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
                    get_data(rank*8+file-8) + S(40, 56);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-20, -31);
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
