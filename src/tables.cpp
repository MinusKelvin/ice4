int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-3, -13, 13, 18, 15, 12, -8, -2};
int16_t DOUBLED_EG[8] = {29, 17, 10, 4, 6, 11, 17, 32};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 21
#define BISHOP_PAIR_EG 43
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 11
#define ISOLATED_PAWN_EG 14
#define VQM_MG 2
#define VQM_EG 3

void unpack_full(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            black_section[70-rank+file] = -v;
        }
    }
}

void unpack_half(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            white_section[7+rank-file] = v;
            black_section[70-rank+file] = -v;
            black_section[77-rank-file] = -v;
        }
    }
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

uint64_t ZOBRIST_PIECES[23][SQUARE_SPAN];
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Piece-square tables
    unpack_full(0, PAWN, "        =WY<9?5'E[SJIF:0BNZXVL90GXgeaVH9Sc|xqfP>E g~ql0d        ", 1.02, 13); // average: 49
    unpack_full(1, PAWN, "        )$,432-3& )+**+**)(''+0.00.+.146:E;@@?@@_No~zvuu        ", 1.333, 89); // average: 115
    unpack_full(0, PASSED_PAWN, "        3;212-.611$% #/432&$'*:<181245EA:/;6=FOVMNBDWY~S        ", 1.266, -19); // average: 5
    unpack_full(1, PASSED_PAWN, "        #%  \"$-\"%,'%%&*&5:3/+-23EG=614:?WVN<7=HNf~Y717DE        ", 1.936, -6); // average: 28
    unpack_full(0, KING, "?A=5=7IL:80&)3<@(.\"#%&/*&/-0,-. ,97:364%:74>95/1G7<=9/\"5~`rg`MVO", 2.101, -54); // average: -2
    unpack_full(1, KING, "3;A?8?9):IRYXQF7FT`dd^REM\\flleYMUfnqsmeSZu{tvzu\\O|yttx~P XW[\\YZ(", 1.78, -100); // average: 2
    unpack_half(0, QUEEN, " )*,,3623441311/?166?JBG@/E71G^`", 1.0, 636); // average: 660
    unpack_half(1, QUEEN, "9% !4.,3:DKHKW^hNjqyWdvvVtq~c`US", 1.353, 1261); // average: 1329
    unpack_half(0, ROOK, "/6:B /56*60236;>CJSYRfjva]x~uw{|", 1.07, 290); // average: 339
    unpack_half(1, ROOK, " ++*((*.,0669=>=AEC?E??9DJCC>BAA", 1.0, 628); // average: 653
    unpack_half(0, BISHOP, "JHGHU[XRT_[Z\\Z^fY]kq_rv~S[a\\8/ 3", 1.123, 193); // average: 255
    unpack_half(1, BISHOP, " 1(049==:EKN7GQS9NMU<HMG:KILQRVQ", 1.0, 362); // average: 398
    unpack_half(0, KNIGHT, "JLPRORWUPYZ]Z_``a`jh`ty~ZZqt 5+c", 1.852, 145); // average: 252
    unpack_half(1, KNIGHT, "(8KOFWZaNahp\\ivx^rw~XennL^ag dt_", 1.237, 274); // average: 350
    
    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 23; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST_PIECES[i][j] = rng();
        }
    }
    ZOBRIST_CASTLE_RIGHTS[0] = rng();
    ZOBRIST_CASTLE_RIGHTS[1] = rng();
    ZOBRIST_CASTLE_RIGHTS[2] = rng();
    ZOBRIST_CASTLE_RIGHTS[3] = rng();
    ZOBRIST_STM = rng();
#else
    auto rng = fopen("/dev/urandom", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
