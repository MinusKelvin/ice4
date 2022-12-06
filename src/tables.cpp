int16_t FT[25][SQUARE_SPAN][12];
int16_t BIAS[12] = {309, 117, 44, -25, -183, 279, 164, 243, -67, 207, 17, 179};
int OUT_W[24] = {-141, 113, 149, -65, 61, -53, 92, 12, -55, -64, -84, 34, 122, -132, -151, 68, -64, 53, -88, 9, 54, 64, 83, -38};
int OUT_B = 2767;

void unpack(int neuron, double base, double scale, const char *data) {
    for (int r = 0; r < 48; r++) {
        for (int f = 0; f < 16; f++) {
            double r_value = (data[r] - ' ') * scale + base;
            double f_value = (data[f+48] - ' ') * scale + base;
            int rank = (f % 8) * 10;
            int file = r % 8;
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
        unpack(0, -13.6, 0.2834, "fjidejniPOPQQQPPRRRPQQSRKKLLNNLKJOUWXVSKwyy{}}~zOIIB8, )SV\\ZX[aa");
    unpack(1, -16.07, 0.3733, "QMLYUOW^\\]Z\\\\\\^\\VTZ\\[[WUZUSPPOQS~zyyxxuw &,13991INQRTVUMHABDFGIL");
    unpack(2, -7.454, 0.3152, "<C>:6A<9PSWWWXVT\\`^]]]_]`egkklidvy{{}~~}jg`WUUZ_LLMMLLMM \"#%(('%");
    unpack(3, -15.16, 0.4864, "CCCBCCCCNOOOPPONMNNOOOONZZZYYZZZ}~~~~~~~hhhiihgf%$\"!!! !cbbbbaab");
    unpack(4, -9.704, 0.3308, "EBBAABACHHHGGGGGKJJJJIIJba`____bqsssrqqpxy|~~|yxvvxxurpq$ !!  !\"");
    unpack(5, -14.85, 0.2949, "g>}#~ j8WSVRUQROPRRUPSRSYQ\\LZH]LWSTSSOPJDEFHGJJERTTRPOUYcb\\]`a_d");
    unpack(6, -6.992, 0.3199, "[Q^af]ZZ),,...,*0.---0//6554344320...---$''\"!$(&@<999=_~I).4643 ");
    unpack(7, 0.341, 0.1647, "+REVzOGX~:k,:$b#7>Y[-5?j5cm4] Tkp/v`N2Zk){UFZ(M)[8^bo*EVLA2AjyWh");
    unpack(8, -13.54, 0.346, "<AZt~g?5h\\ababSd<4>IJ<6Bf\\TGEQ[sHBBJHCEM 2A]fM:5BHGFFGGBTRRQQNMP");
    unpack(9, -12.81, 0.4009, ">??CA;<?BB@AA>A?FEEFEFCFA?@AA>?@||}~}||} \"',--&$*168>CB5VGFLQSX^");
    unpack(10, -12.52, 0.5703, "CHG95* (7596534627554508014669>>(+0159=@~wiD91#&66553-(64003321,");
    unpack(11, -11.98, 0.967, "/0////0.8776666777666677;::9999;CCBBBBCC2234443398789:::@~H&#!  ");

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
