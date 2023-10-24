#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int QUADRANTS[] = {
    S(218, 228), S(241, 231), S(225, 226), S(249, 229),
    S(241, 321), S(250, 326), S(242, 322), S(255, 326),
    S(293, 575), S(319, 595), S(301, 566), S(339, 582),
    S(653, 1110),S(654, 1151),S(656, 1109),S(673, 1162),
};

#define BISHOP_PAIR S(24, 48)
int32_t DOUBLED_PAWN[] = {S(6, 23), S(-10, 21), S(10, 20), S(14, 15), S(14, 11), S(11, 22), S(-7, 20), S(6, 33)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(10, 9)
int32_t PROTECTED_PAWN[] = {0, S(8, 7), S(9, 8)};
#define ROOK_OPEN S(34, 12)
#define ROOK_SEMIOPEN S(16, 17)
int32_t PAWN_SHIELD[] = {S(1, -13), S(11, -30), S(14, -26), S(23, -24)};
#define KING_OPEN S(-43, -3)
#define KING_SEMIOPEN S(-11, 20)

int get_data(int i) {
    auto DATA_LOW = "<OQ4-1( ?IB:65'$AEQMJ>-)LN`]TH<5^c)oigHCB'['}`>i7H=:117;95+)'(.<0:,#+,?F0;=;?FPJ% :AGJXUKZ]NVi$K99)o#q?HF6#bh 9Hu{MFKNkdR];0 @D+_od>,GL=k*c]x9,{j){H%o{0TLin>0NF &./+/@9/CCM;:OO#% \"099./<99,0<H/159 ,2.*70/&',, ().028487531312#+6D;945 \"046-*)$*())-0,03/+.68:BT=IMEPF~{?GMOOR1-& ')5-2?3.(0<3RUJB89DHmm[KFFT_8/!XPes&7>f.)2BHz,97)7,oy-9HF9+x*>Q[ZM>1:L_jk[J?>Yeorf_ECahrigf?2`\\P[`Z'7#}=3)+  Jdj^nu}f|(/r*38 0'-%3496:FG0CGJ#%($%$%$  $#$((%%\"%# %*5&<NK1FS_";
    auto DATA_HIGH = "                                  !        !                                                  ! !!! ! !!!!!  !!!                         !   !!  ! !!  !!!! !!!\"                                                                                                          !!!!!!                                !!!    !!!       !!!!!!  !!!!!! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ! !!!!           !! !!!                                                ";
    return DATA_LOW[i] + 95 * DATA_HIGH[i] +
        0x10000 * (DATA_LOW[i+224] + 95 * DATA_HIGH[i+224])
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
                PST[WHITE | KING][10*rank+file] = get_data(rank*8+file+96)
            );

            if (rank > 0 && rank < 7) {
                PST[WHITE_PAWN][10*rank+file] = PST[BLACK_PAWN][70-10*rank+file] =
                    get_data(rank*8+file-8) + S(25, 76);

                PST[WHITE_PASSED_PAWN][10*rank+file] = PST[BLACK_PASSED_PAWN][70-10*rank+file] =
                    get_data(rank*8+file+40) + S(-17, -9);
            }

            for (int piece = KNIGHT; piece <= QUEEN; piece++) {
                PST[BLACK | piece][70-10*rank+file] = -(
                    PST[WHITE | piece][10*rank+file] = get_data(
                        (rank & 4 ? rank ^ 7 : rank)*4 + (file & 4 ? file ^ 7 : file) + piece*16+128
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
