int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-3, -13, 13, 18, 15, 12, -8, -1};
int16_t DOUBLED_EG[8] = {29, 17, 11, 3, 5, 11, 16, 32};
int16_t VIRTUAL_QUEEN_MOB_MG[] = {17, 8, 5, -1, -10, -24, -15};
int16_t VIRTUAL_QUEEN_MOB_EG[] = {19, 20, 15, 11, 3, -16, -48};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 11
#define ISOLATED_PAWN_EG 14

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
    unpack_full(0, PAWN, "        ?Z\\?=@7(G]UMJH;2DP[ZWN:2HYhfbWI:Td|xsgQ@D f~rl0d        ", 1.028, 11); // average: 49
    unpack_full(1, PAWN, "        )&.643-3& ),*)**)((&'+/.0/-*-046:F;?@?A@`Jo~zvux        ", 1.304, 90); // average: 115
    unpack_full(0, PASSED_PAWN, "        3<211--612%% \"/432&$&)9<181235DA:/:5<ENVNMCCVX~T        ", 1.269, -18); // average: 6
    unpack_full(1, PASSED_PAWN, "        $% !#%-$','%&'+'6:30,.34EG>725:?WUN<7=HMd~Y828EE        ", 2.023, -9); // average: 27
    unpack_full(0, KING, "@A<4<7HL<80&)3=A+/ !\"%0,)0*,'+.\"/:56.45)=93<7414I:>?;1%8~athbOVO", 2.141, -56); // average: -3
    unpack_full(1, KING, "6=B@9@;-?KSZYRH=ITcgg`SHN\\irrhZOUfrxzqeS[t~xz}t\\O{yttx}Q WW\\\\YZ(", 1.785, -104); // average: 1
    unpack_half(0, QUEEN, " )*,,3623440311/?066?JAG@.E71G^a", 1.0, 636); // average: 660
    unpack_half(1, QUEEN, "9% !4-,2:CJHKW^gNkqxWevvVtp~caUS", 1.353, 1260); // average: 1328
    unpack_half(0, ROOK, "/6:C /56*60236;>DJSYRfjva]x~uv{|", 1.072, 290); // average: 339
    unpack_half(1, ROOK, " ++*((*.,0669=>=AEC?F??9DJBC>BAA", 1.0, 627); // average: 652
    unpack_half(0, BISHOP, "JHGGU[XRT^[Z[Z^fY]jp`rv~S[a\\7/ 3", 1.119, 193); // average: 254
    unpack_half(1, BISHOP, " 1(/48==:DKM6FPS9NMU<HMF:KILPRVQ", 1.0, 362); // average: 397
    unpack_half(0, KNIGHT, "JLOROQWUPYZ]Z_``a`jh`ty~ZZqt 5+d", 1.852, 145); // average: 252
    unpack_half(1, KNIGHT, "(7KNFVYaNahp\\hvx^rw~XdnnL^`g ds^", 1.235, 274); // average: 350
    
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
