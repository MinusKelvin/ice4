int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

int16_t FT[10][25][4];
int16_t FT_BIAS[] = {15, 64, 101, 36};

int16_t OUT[64];
#define OUT_BIAS -74

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
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Unpack FT
    const char *ft_data = "7=kH%5d=%@eD&BhB'DmC Em;3Ni.26rL8,S,76j-59i253i140h44+j15)k053l.8*P-68h056j.55i/40h44-i33,j.4-l+.7Y8.=q2.4j2/.g.1*f.3'f*3&e+4\"h'6:t 5:q$83n&9-n'9(m,3*~04 d-6 x&:H8A7Qk=5GX@5P_C<^iD<^p9CmiCDcg98>d9<L =:?YA7D^A2HaD/B`>/L`A-Ce,FXmY4ViW2YmS2XkQ2UmL5VkJ5ZlM5^mY7Zeb4WhY3[qS2VjR3WlU3TjQ3UkK5ZlZ4bOp/[Xh0_ib2]ib5[h^7XgU8VgQ8RfO{z`~{t_xy~npxvmqvsmowqkl~njiurip<eu94faB/exB/cwL0NkF1IkA1Hm7a?n@";
    double ft_scale[] = {1.041, .7243, 1.69, .7533};
    double ft_offset[] = {-21.95, -27.47, -126.4, -21.37};
    for (int color : {WHITE, BLACK}) {
        for (int piece = PAWN; piece <= KING; piece++) {
            for (int rank = 2; rank < 10; rank++) {
                for (int i = 0; i < 4; i++) {
                    int v = *ft_data++ - 32;
                    FT[rank][piece | color][i] = v * ft_scale[i] + ft_offset[i];
                }
            }
        }
    }

    // Unpack hidden layer
    char out_data[] = "!2q*+2n\"11g!5*d%3'c(0$i.('n. -q.zo)nqo-uko3vfx6shz7ol~1jt{-i{u*j";
    for (int i = 0; i < 64; i++) {
        OUT[i] = (out_data[i] - 32) * 3.714 - 170.8;
    }

    // Unpack piece-square tables
    const char *pst_data = "b~~~Y|(a~~;3_qqza~(cw}:K^hgxkh)dbzDJb_]vr^\"\\XvJ46eqH`SL^pU< O`oJjJWdmUE,NckOx>XblZR7?^fP|2\\dg]]>dofRoJhja]JN~rfQsIxrb\\YS|qcN|3xud^iU|q`L~*|xb]jSIwhLo1Uu`]QN\\wbFm7exbZcVftaFw0`~eYpXlqbDx+^{aUxW;xhTn0TqbcQbYodNi8dvj^llevhNu1dwd[xdqpiMw/eweV~\\!mlVK.`dkb9cCwoVT-zeiYOv]ymTb(whdUdmnzmUh+xbWNk\\ WQCN5[N?IEJ/QP:P sSDD[~>fQGd.~R?Bfr\\fLId:|S;Aqg_\"7$ |  && -e') 2m$J! &g\\  'No+U$\"-_eP&(Ti*G #5V";
    double pst_scale[] = {1.521, 2.634, 3.312, 3.192, 3.242, 2.451, 1.641, 2.713, 1.919, 3.661, 3.378, 1.725};
    double pst_offset[] = {-95.65, -28.33, -25.39, 24.99, 51.17, -129.3, -7.524, 183.8, 323, 522.4, 1143, -83.14};
    for (int rank = 0; rank < 80; rank += 10) {
        for (int file = 0; file < 4; file++) {
            int i = 0;
            for (int phase : {0, 1}) {
                for (int piece = PAWN; piece <= KING; piece++) {
                    int16_t *white_section = PST[phase][piece | WHITE];
                    int16_t *black_section = PST[phase][piece | BLACK];
                    int v = (*pst_data++ - ' ') * pst_scale[i] + pst_offset[i];
                    white_section[rank+file] = white_section[rank+7-file] = v;
                    black_section[70-rank+file] = black_section[77-rank-file] = -v;
                    i++;
                }
            }
        }
    }

    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
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
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}

int clipped_relu(int v) {
    return v > 127 ? 127 : v < 0 ? 0 : v;
}
