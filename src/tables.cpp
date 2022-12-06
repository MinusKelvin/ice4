int16_t FT[25][SQUARE_SPAN][12];
int16_t BIAS[12] = {-57, -122, 220, 251, 163, -6, 87, 112, -16, -192, 159, 270};
int OUT_W[24] = {-54, 65, 52, 71, 72, -72, -199, 51, 121, -152, 69, 69, 56, -65, -49, -67, -79, 72, 197, -53, -119, 151, -70, -69};
int OUT_B = 1382;

void unpack(int neuron, double base, double scale, const char *data) {
    for (int r = 0; r < 48; r++) {
        for (int f = 0; f < 16; f++) {
            double r_value = (data[r] - ' ') * scale + base;
            double f_value = (data[f+48] - ' ') * scale + base;
            int rank = (r % 8) * 10;
            int file = f % 8;
            int color = f / 8 ? BLACK : WHITE;
            int piece = r / 8 + 1;
            FT[piece | color][rank + file][neuron] = r_value * f_value;
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
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;

void init_tables() {
    // Feature Transformer
    unpack(0, -0.994, 0.33, ">F:%!!!%\"%$$\"   67101111$$\" !\"\"#npu|~}~|/75/+)(+#\"\"!!!%$RPPNMNRV");
    unpack(1, -9.467, 0.2691, "~YSNPRNYIJPSZ[btFFKTZQRdLHFKWalpNPTbhepz .0;K>7;MNU[[UMKTQONLLKR");
    unpack(2, -10.59, 0.3815, "GDCCEFH`SVY\\[ZYXW[^^^^__xz|}~{y|38>AA><>8?FHJOMIQSTSSSSQ\" !!! !\"");
    unpack(3, -10.59, 0.3727, "U?==>@@KOMLMNMNNNKKJJIIIhigfedde}}|||||~ %+1;>?>\\\\[[\\[\\\\!!!\"\"\"\"\"");
    unpack(4, -10.21, 0.4726, "J578887QKIILLNOPIIHJJKKMQPPRSTTT~{yzz{|}LKJLMPQOONONNNNN !!   \"!");
    unpack(5, -8.451, 0.2489, "m~|`O:;[VSVOIIDIPPSKD<48XLPOQRVRo\\^VRNSS =^SIKNQPKKGD@88+,5AILIQ");
    unpack(6, -8.289, 0.2326, "Ihji[TY~WVaVYRK?IRYZWXNKY_ZUPPPLcb^[X^XVshjYE6- =89:;@AHSWVUTYYT");
    unpack(7, -8.727, 0.3344, "h?AADFGCMKIFIORUONKKMPVU^\\]]aacdqrqpu{~|GDAB/(*-`^^]\\[\\^ !\"!\"\"!!");
    unpack(8, -20.93, 0.361, "sUUPG: qiifgg_[Dryuvstmo_\\]^]\\Y\\Yde^WVPV~vndY@6FggfffhhkYYYXWYWV");
    unpack(9, -2.235, 0.2273, "1Q5'%'(1$./+((((&1-+++,+( \"!!! #V\\_a_UTU~~pkinw}*)*,,))*ZXVVUTX\\");
    unpack(10, -4.796, 0.1804, "c^]]\\PF~;9<=;-, ;6<9531=TUPLGJKCNB><:6<:25=8(,)!Z>=78>F}(-8<97/0");
    unpack(11, -7.075, 0.3292, "V:::BPUN41431:788:9886A28879886,3-,+*&' 4286@[e~FFA:6,(!;>=83.(,");

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
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
