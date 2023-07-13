int PST[25][SQUARE_SPAN];

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "$$&\"% $'3EA>?NQ89GECGGS<9FHLNLK:=MKRSSP<CWYcaaUDvms~u_:S\"##\"%$'$ ! !## !KLKLKJLHGJEGGGHFJKFDEGKGPPLIHJNM`b\\VVW`Zx|uklv~x  #! #\"#FMG7=7NRHA5**5?G02\"! #/,-/+)%(, /914,23&:7.:21,/E8783,!7~dui[PWO+01,'1- 3;BGGA91>FNQQMF?FPVYXUPHQ]`_a`^R[nofior^Swsklt~X#XWVXZ_-#*,-,', -152389/030105:82-/,24486-239;7B4>7EIVQI<&:79O=G,EXdetN@B5+))  89=496*#7AEQJLKD;FZ^lc`YUNfouxssZUgrvwxpeQsu|~owda_ZSUPeh/6>BE<<5$-45763 *1/152;/039=@<=8>DKUYURHK\\]n{sqXYXp~yy_fpry{{yyz-655010 /3775.)+4<?>:90-CIIEBB<9LSPJFGECQMOG?CAGNUOLLEMEEIGHGGDCMOLLIJIMYZWQTVYRT]Z[X[[U[Y]bb[Z[VZemji^[Sigvx~qeUW\\Z_dbX<7C1H =I&3!2211 49A??;:.;GNOMIB;:HRSROG5;RLSUOQ>CJPFHJM>8MIPJHH:SOOUQXNGIKOOPLLHJLQRSUONMTVZYWWPUYZ[]]]YZ[cebf__Zkls~zq^TVgrkuW] 1>Ul!=8)3EMLJ;$BXX__VRAJ^enne_N[fvvwugZ]ovz~xs`Yfprime[M_dhh^_M eod[vc\"";

void unpack_full(int phase, int piece, double scale, int offset) {
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | BLACK][70-rank+file] += -v * phase;
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

struct Zobrist {
    uint64_t pieces[25][SQUARE_SPAN];
    uint64_t ep[120];
    uint64_t castle_rights[4];
    uint64_t stm;
} ZOBRIST;

void init_tables() {
    unpack_full(1, PAWN, 1.542, -5); // average: 49
    unpack_full(0x10000, PAWN, 3.029, -6); // average: 113
    unpack_full(1, KING, 1.908, -55); // average: -7
    unpack_full(0x10000, KING, 1.683, -75); // average: 6
    unpack_full(1, QUEEN, 1.0, 579); // average: 604
    unpack_full(0x10000, QUEEN, 1.466, 1214); // average: 1291
    unpack_full(1, ROOK, 1.123, 280); // average: 331
    unpack_full(0x10000, ROOK, 1.0, 590); // average: 622
    unpack_full(1, BISHOP, 1.384, 190); // average: 267
    unpack_full(0x10000, BISHOP, 1.0, 345); // average: 381
    unpack_full(1, KNIGHT, 2.03, 136); // average: 247
    unpack_full(0x10000, KNIGHT, 1.236, 259); // average: 334
    
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
