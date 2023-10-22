#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
int MATERIAL[] = {S(28, 79), S(38, 108), S(215, 238), S(208, 255), S(274, 482), S(699, 989), 0};
#define BISHOP_PAIR S(23, 49)
int DOUBLED_PAWN[] = {S(8, 22), S(-10, 18), S(9, 20), S(14, 13), S(15, 10), S(11, 22), S(-7, 18), S(8, 32)};
#define TEMPO S(11, 11)
#define ISOLATED_PAWN S(10, 9)
int PROTECTED_PAWN[] = {0, S(8, 7), S(9, 7)};
#define ROOK_OPEN S(32, 11)
#define ROOK_SEMIOPEN S(16, 13)
int PAWN_SHIELD[] = {S(22, -19), S(33, -38), S(36, -35), S(45, -33)};
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-12, 19)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "        <PO2-0' @I@864&$CEPLJ=,*LO`]SF:5`f~oifFD@)p~~rRu                &,8F<;56\"$388/-+%+*++/2.03/*/79:@SDJNGQF~x~~~~~~                .OI- !  /<+#    7C@34+-1?L_ZTOL@Wb~sssc\\~~~~~~~~                                /6,$ !-.TWB.,4DQ~~sWSay~~~~~~~~~        -09:6;JD9MMTGXZ]YZro[{~~HUxu   Q$0LQLT[dMbmv[kz}\\s{~QarvF\\\\b P_XMNGKZ`aUUc_`X]bpYct}jy~~Regb/* !K`Y^[eglimy|dv}~jzx~lqyr`rpuuy}{QTY]APUPLXONPPSV[dhhi|{~on~~||~~adfbb^aadchinrqovutqursow~}|{}~}QXZ^]eiecedabf`bpfmo}~}~n_vsLex|<.*.302?@S_[Yhu~Xy~~Yi~~\\w~~ojtrbX<<yiR7;C!  $  \"/& A[L==Q?U TV:!;D: 1?K7DU_?RakIcmrLmqu>kjc K?J";

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
            int mg = DATA_STRING[rank*8 + file] - ' ';
            int eg = DATA_STRING[rank*8 + file + 64] - ' ';
            int v = mg + eg * 0x10000;
            PST[WHITE_PASSED_PAWN][rank*10 + file] = v + MATERIAL[0];
            PST[BLACK_PASSED_PAWN][70-rank*10 + file] = v + MATERIAL[0];

            mg = DATA_STRING[rank*8 + file + 128] - ' ';
            eg = DATA_STRING[rank*8 + file + 196] - ' ';
            v = mg + eg * 0x10000;
            PST[WHITE_PAWN][rank*10 + file] = v + MATERIAL[1];
            PST[BLACK_PAWN][70-rank*10 + file] = v + MATERIAL[1];

            for (int piece = KNIGHT; piece <= KING; piece++) {
                mg = DATA_STRING[piece * 64 + rank*4 + (file & 4 ? file ^ 7 : file) + 128] - ' ';
                eg = DATA_STRING[piece * 64 + rank*4 + (file & 4 ? file ^ 7 : file) + 160] - ' ';
                v = mg + eg * 0x10000;
                PST[piece | WHITE][rank*10 + file] = v + MATERIAL[piece];
                PST[piece | BLACK][70-rank*10 + file] = -v - MATERIAL[piece];
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
