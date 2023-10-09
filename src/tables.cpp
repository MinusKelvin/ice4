#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(23, 49)
int32_t DOUBLED_PAWN[] = {S(6, 23), S(-10, 21), S(10, 21), S(13, 15), S(14, 12), S(11, 23), S(-7, 20), S(7, 33)};
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(10, 9)
int32_t PROTECTED_PAWN[] = {0, S(8, 8), S(9, 8)};
#define ROOK_OPEN S(33, 12)
#define ROOK_SEMIOPEN S(16, 17)
int32_t PAWN_SHIELD[] = {S(1, -13), S(11, -29), S(14, -26), S(23, -24)};
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-12, 19)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = "        FTU@<>83IPJDCA85JLURPH<:QS_]WOFB^a{jfeOLM8`~zcJj                EHMSONMMDEKMNJHHFHHHHJKIKLJHJMNOR[QVWTYTjkwy{}~}        JLPQQQNLJOVTVZSRNVY\\]Z\\ST[^^b``YZ]ekfm`aZhns~}qbSZdngw_[ -BPd2)8=>TYZWF<S]bhe`XVUfmtsleWbkyyxtl^doxz~us][fqnfcbVRcdebZ[M,cj_Yab JNJKKHLOWX^QUX^UP]Z[Z[]VTX]ff\\YVQ\\gskk\\XQegut~rgN^YW_f^T8,2#2 <5'9&5484 37;BA?=.AEPQQL@:<OUVROH5CPLVTOO=HGNCEMF@6GJMGAH5MOPRMTKD-.38>::5 &,,1:<\"%*''03B4'#.265;8.6<EGRRF9ILTgpz[CB[e]w`bSO[[esig4361+/+#..30($ (368700&&>E@>6933IHH@8567GFE<73.7IOHBD;=8GMKJDCBC(.254/2'18@<<@@859997=D>6565=:@B68<>?FBI=GCNVj`b@2>58YGO 6Rg\\gV07.+-+#!,03/76+ &5BOGMKB7?SZi^]SOCVbnrjhVDJcfll`S?Xht~e_]NPTKQNIZSRKAIAUZYQI;=GSYEG2/12?=5:,& -0$:@;*\"/2*@L<9FTNF=KGYJBGM\\Ye?QL[~QY_^W_XJPY`gf`XOWbmrrkc[`jtz{ricbqw}~xtfeuy~zxxc\\usmruqV-URb]XY 333333335D;9007:84+('(.<09,#*+?F/9;9=DNI$ 9@FIXTEWVFKb~E33333333'''''''',*% $'/*.7.*%+4.FH?923;>ZZMA==GOyrgJDS^k|~Z1/3=G''''''''";

void unpack_full(int phase, int piece, double scale, int offset, int colormul = -1) {
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | BLACK][70-rank+file] += colormul * v * phase;
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
    unpack_full(1, PAWN, 1.425, 0, 1); // average: 51
    unpack_full(0x10000, PAWN, 2.34, 0, 1); // average: 91
    unpack_full(1, KNIGHT, 2.293, 124); // average: 254
    unpack_full(0x10000, KNIGHT, 1.398, 222); // average: 312
    unpack_full(1, BISHOP, 1.385, 185); // average: 260
    unpack_full(0x10000, BISHOP, 1.0, 312); // average: 348
    unpack_full(1, ROOK, 1.0, 298); // average: 333
    unpack_full(0x10000, ROOK, 1.0, 558); // average: 583
    unpack_full(1, QUEEN, 1.0, 599); // average: 630
    unpack_full(0x10000, QUEEN, 1.41, 1127); // average: 1190
    unpack_full(1, KING, 2.293, -96); // average: -15
    unpack_full(0x10000, KING, 1.872, -129); // average: 0
    unpack_full(1, PASSED_PAWN, 1.008, -19, 1); // average: 7
    unpack_full(0x10000, PASSED_PAWN, 1.331, -10, 1); // average: 26
    
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
