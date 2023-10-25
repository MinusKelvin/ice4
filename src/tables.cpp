#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(218, 404), S(251, 417), S(228, 402), S(262, 416),
    S(334, 500), S(344, 510), S(335, 503), S(356, 508),
    S(390, 907), S(426, 949), S(403, 894), S(459, 929),
    S(901, 1694), S(891, 1769), S(906, 1699), S(925, 1787),
};

#define BISHOP_PAIR S(28, 91)
int32_t DOUBLED_PAWN[] = {S(9, 44), S(-16, 38), S(18, 37), S(19, 21), S(24, 13), S(14, 36), S(-8, 31), S(10, 62)};
#define TEMPO S(13, 21)
#define ISOLATED_PAWN S(13, 11)
int32_t PROTECTED_PAWN[] = {0, S(12, 14), S(14, 14)};
#define ROOK_OPEN S(48, 25)
#define ROOK_SEMIOPEN S(26, 21)
int32_t PAWN_SHIELD[] = {S(23, -52), S(34, -65), S(36, -48), S(46, -45)};
#define KING_OPEN S(-67, -3)
#define KING_SEMIOPEN S(-17, 23)

int get_data(int i) {
    auto DATA_LOW = "Knn7,=4 RhRG?E1*PYkjbS:/^p%}p_T@!\"Q4,$YQ$8$i7VK&>U=F9:<N@;3&+(;K<4, 11QY9ICCLZjc+1FSbd|u:[{R!^6k@qrIm\" UwYN@jW%p!u!\"\")?6&JFX?:YU3?\" ?LI8<OIH<=KZ<?FR!8@=1H<@3178#*!,<7A9><766361'1@XLF:A !<:>0.-+9/')196<?=15CDIUvWgk^p_r,Qkd)0]1#&!5):%6C42\".=0lmcS@BNXC<$cWTq&*hYra.FbXFXd]HuS'J>!D|sAr>=vV#1`!Goycy$8t5I]'Obl!8+8,<DKERbh7`ls -1'1.0/#%-($3410.B:!6:K8ZwpEmx7";
    auto DATA_HIGH = "                          !     !!!!!!    !!!  !                                        !   !!! !  !        !!!!  !!!!!!!!!!!!!!                                                                                         !!!!\"\"!                        !!!    !\"!!  !!!\"\"!    !         !!  !!       !! !!!!!!!                                               !";
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
                    get_data(rank*8+file-8) + S(42, 138);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-34, -7);
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
