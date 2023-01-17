int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {7, -8, 15, 18, 19, 16, -2, 8};
int16_t DOUBLED_EG[] = {34, 25, 17, 11, 10, 15, 19, 35};
int16_t KING_OPP_PAWN_TROPISM_MG[] = {40, -10, 0, 1, -1, -1, 16, -86};
int16_t KING_OPP_PAWN_TROPISM_EG[] = {67, 17, -2, -20, -36, -39, -27, 5};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 41
#define TEMPO_MG 6
#define TEMPO_EG 3
#define ISOLATED_PAWN_MG 9
#define ISOLATED_PAWN_EG 9
#define CONNECTED_PAWN_MG 7
#define CONNECTED_PAWN_EG 7
#define ROOK_OPEN_MG 28
#define ROOK_OPEN_EG 9
#define ROOK_SEMIOPEN_MG 17
#define ROOK_SEMIOPEN_EG 17

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
    unpack_full(0, PAWN, "        7LQ2-.& 9IA821%%9ALIC:*)@IWTND82IOjb]R@7U7l~ka<i        ", 1.052, 30); // average: 59
    unpack_full(1, PAWN, "        #&+0.-*+! ())&&%&)((&(+(03211131<KCIGBA;dft~ztxs        ", 1.637, 95); // average: 128
    unpack_full(0, PASSED_PAWN, "        0?741//5/1%' #040,\"!&*;=/5/059ICA/<7BMV_LG?AZ_~V        ", 1.069, -16); // average: 5
    unpack_full(1, PASSED_PAWN, "        %& !##+!+3,(&'+&?F>70156RVK?8:?Gge]E=COYm~c>48=C        ", 1.58, -6); // average: 28
    unpack_full(0, KING, "HHA6?:PVG@6),9EM13# \"'43'-&# (.\")6.+#01%8912/360L>>523*;~dve\\KYH", 1.864, -55); // average: -8
    unpack_full(1, KING, ">HJC>HD5BMSZYSJ@FPY]]WNFKV[``YRJW_cega]R]qrmopp]T|xutw~T _\\bb_a+", 1.392, -77); // average: 1
    unpack_half(0, QUEEN, " (*,+3522330101.=/45=H?E8,A4+B[^", 1.0, 642); // average: 664
    unpack_half(1, QUEEN, "8& \"3*)08BHEKU\\eLipwUcuuXro~fbUS", 1.332, 1267); // average: 1332
    unpack_half(0, ROOK, "149? *1/+4,+/044=BJOH[_iPNkphlsq", 1.0, 286); // average: 324
    unpack_half(1, ROOK, " %%\"#!#''*0/6::7>B?:C<<5DH??>@=<", 1.0, 625); // average: 646
    unpack_half(0, BISHOP, "MJIIV]ZTT`][\\[_hZ^kq`rw~Q\\a\\:/ 1", 1.133, 192); // average: 255
    unpack_half(1, BISHOP, " 2)057<<9DJM6FPR8MLT;GLF8IHLNRVQ", 1.0, 366); // average: 401
    unpack_half(0, KNIGHT, "JLOQNQWUPZZ]Y_``a`jh_sx~WYpt 3)c", 1.821, 148); // average: 252
    unpack_half(1, KNIGHT, "*<NRJX[bPbjq]jwy^rx~XennM^`g ds_", 1.249, 274); // average: 352
    
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
