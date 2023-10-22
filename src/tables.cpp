#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];

int MATERIAL[] = {S(28, 79), S(38, 108), S(215, 238), S(207, 255), S(274, 481), S(699, 988), 0};
#define BISHOP_PAIR S(23, 49)
int DOUBLED_PAWN[] = {S(8, 22), S(-10, 18), S(8, 20), S(14, 14), S(14, 11), S(10, 22), S(-7, 18), S(8, 32)};
#define TEMPO S(10, 11)
#define ISOLATED_PAWN S(10, 9)
int PROTECTED_PAWN[] = {0, S(8, 7), S(9, 7)};
#define ROOK_OPEN S(32, 11)
#define ROOK_SEMIOPEN S(16, 13)
int PAWN_SHIELD[] = {S(20, -21), S(30, -40), S(33, -37), S(43, -35)};
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-12, 19)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "        ;OO1-0' >H?764%$BDOKI<,)LN_]SF:4_e~nieEC@)k~~rPt                &,9F<:56#$388/,+&++++/2.13/+/79:ATCJNGQF~x~~~~~~                .NI. !  .<+#    7C@34+-1?L^ZTOL@Wb~srsb\\~~~~~~~~                                /6,$ !-.TWB.,4DQ~~sWTay~~~~~~~~~        -1:;7<KE:MNUHY[^YZspZ{~~HTxu   Q$0LRLT\\dMbmv[k{}\\s{~QasvG\\\\b Q_XMNHKZaaUVc``Y]bpXct}jx~~Reec/* !K`Y_[eglimy|dv}~kzx~lqyr`squuy}{RUY\\BQUPMYONQPSV[dhhi|{~on~~||~~acfcb^aacchimrqovttqursow~}|{}~~RY[_^eifcfebbgacpfmo}~}~n_vtMfy}<.).3/2@@S_[Ygu~Zy~~Yi~~\\w~~ojtsVTD,?.ZdqcP27OhwAH   !=6%/    ! .A4   \" 8[42EaWD-RC\\D6MK EY.BDb (;GF8F<  1=KI>/ 1CU]]QC6>Qaih]N<D_kppjcGLipwpnnIAmf]ejf8 G;JFAL ";

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
            int mg = DATA_STRING[rank*8 + file];
            int eg = DATA_STRING[rank*8 + file + 64];
            int v = mg + eg * 0x10000;
            PST[WHITE_PASSED_PAWN][rank*10 + file] = v + MATERIAL[0];
            PST[BLACK_PASSED_PAWN][rank*10 + file] = v + MATERIAL[0];

            mg = DATA_STRING[rank*8 + file + 128];
            eg = DATA_STRING[rank*8 + file + 196];
            v = mg + eg * 0x10000;
            PST[WHITE_PAWN][rank*10 + file] = v + MATERIAL[1];
            PST[BLACK_PAWN][rank*10 + file] = v + MATERIAL[1];

            mg = DATA_STRING[rank*8 + file + 512];
            eg = DATA_STRING[rank*8 + file + 512];
            v = mg + eg * 0x10000;
            PST[WHITE | KING][rank*10 + file] = v;
            PST[BLACK | KING][rank*10 + file] = -v;

            for (int piece = KNIGHT; piece < KING; piece++) {
                mg = DATA_STRING[piece * 64 + rank*4 + (file & 4 ? file ^ 7 : file) + 128];
                eg = DATA_STRING[piece * 64 + rank*4 + (file & 4 ? file ^ 7 : file) + 160];
                v = mg + eg * 0x10000;
                PST[piece | WHITE][rank*10 + file] = v + MATERIAL[piece];
                PST[piece | BLACK][rank*10 + file] = -v - MATERIAL[piece];
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
