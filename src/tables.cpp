int16_t FT[25][SQUARE_SPAN][4] = {0};
int16_t PST[25][SQUARE_SPAN] = {0};
int16_t FT_BIAS[4] = {-1, -1, 158, -115};
float OUT_WEIGHTS[16];
#define OUT_BIAS 1.449
int OFFSET[] = {-5, -9, -82, -22};

void unpack_ft(int piece, const char *data) {
    int16_t (*white_section)[4] = FT[piece | WHITE];
    int16_t (*black_section)[4] = FT[piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            for (int i = 0; i < 4; i++) {
                white_section[rank+file][i]
                    = white_section[rank+7-file][i]
                    = black_section[70-rank+file][i]
                    = black_section[77-rank-file][i]
                    = *data++ - ' ' + OFFSET[i];
            }
        }
    }
}

void unpack_pst(int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[piece | WHITE];
    int16_t *black_section = PST[piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ' + offset) * scale;
            white_section[rank+file] = white_section[rank+7-file] = v;
            black_section[70-rank+file] = black_section[77-rank-file] = -v;
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

uint64_t ZOBRIST_PIECES[25][SQUARE_SPAN];
// uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST_PIECES[i][j] = rng();
        }
    }
    // ZOBRIST_CASTLE_RIGHTS[0] = rng();
    // ZOBRIST_CASTLE_RIGHTS[1] = rng();
    // ZOBRIST_CASTLE_RIGHTS[2] = rng();
    // ZOBRIST_CASTLE_RIGHTS[3] = rng();
    ZOBRIST_STM = rng();
#else
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
//     fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif

    unpack_ft(PAWN, "G%%8>& ?>'+3I\"!7q'&2^(\"7Z(\"6\\&%2h'&5Q'\"6Q&%6N'$5i''2W'%1R'%4J'%6o*+/Y)(/X,(0N(&5y+.+c,,)[--,U+)1y+0*y+4$k+.(],+.H/&7@,':E'\"5= ,8");
    unpack_ft(KNIGHT, "@21AJ>8FC'$@@'$>A)#>C'#9E(#;H'%:H)%=G)$;K(%;H(%9F+%;E)%8J(%7M(%7D-&;K*$8F*$8K(&8?2+87-&:91)9;*%;?2+7D406;2*:=40:l9--K?:.V9:*C<48");
    unpack_ft(BISHOP, "K*(?P31CL'#GG&%=D'&AK(%BH'$>I'$;L(%?H(&=K'%;M'&:D)%<I(%:N'&:K(&:H+&;L)%7E*%9F(&:G/&;?.':>/'9>3.<G1+8@.*7C3.8E729c;73GIB:L@=7IH?:");
    unpack_ft(ROOK, "e:74_2/4X-+8R*';f55/\\743U526Q736f533[516X845U855g952[;74X964V:65a>93W<82T>94P;76X?:3K<84K=74H<67U?:3P<82G;84E=96<B<9=A;7<A<6?A;8");
    unpack_ft(QUEEN, "]cUzOeU~Li\\lMk`TPeTsIdTsIgXmKiZ`NfUgNfTjPeVbNfVkRhTiSeTgTgV`XgWeUeOiWgQnWiUgYjVkSiOjPjRkPpSkSlZkSjRgUoTiQjSqUo[pZx_YKr[iEoYmHs`b");
    unpack_ft(KING, " )!H'(!P,&\"d1)!F''$D3'$>@&$6M&$/;(%7E'&1Q'%.S&$,C'%+M'%(W&%#Z&'#3'%!8-+!G-+!P&%!*.+!,2.!-2/ C/.!&+'!+,(!,*)!710!%+*#'1+!1,,!310!");
    const char *data = "\\k{j[~fgC2&4D :7";
    for (int i = 0; i < 16; i++) {
        OUT_WEIGHTS[i] = (*data++ - ' ' - 47) * 0.1448;
    }

    unpack_pst(PAWN, "'& 'EZYYD_YZF\\ZZAYP]H`Xbpiz~' &#", 1.906, -6); // average: 41
    unpack_pst(KNIGHT, "? ejdomncmntjsvwjtzy^sm~ZUf\\42=D", 3.625, 15); // average: 76
    unpack_pst(BISHOP, "dFepnouuouwytwxznz{~ksud`obT< 6&", 3.605, 20); // average: 84
    unpack_pst(ROOK, " Uo~4:F@<J;<3.=7#;5?*FHK1FPH09:5", 1.804, 262); // average: 206
    unpack_pst(QUEEN, ", *RFNJWcctY_nwh~gjXspYSiRa.):P=", 1.405, 276); // average: 237
    unpack_pst(KING, "KA ANQUVPUUY\\]_]o_[mgbb]r~~^bgm[", 2.766, -76); // average: 9
}

int clipped_relu(int v) {
    return v < 0 ? 0 : v > 109 ? 109 : v;
}
