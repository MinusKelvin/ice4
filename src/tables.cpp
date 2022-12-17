#define NEURONS 8
int16_t FT[25][SQUARE_SPAN][8];
int16_t BIAS[NEURONS] = {166, 109, 181, -151, 147, 33, 80, 4};
int OUT_W[2*NEURONS] = {-101, -18, -148, 21, -292, -57, -54, 36, 102, 19, 151, -21, 291, 57, 54, -34};
int OUT_B = -161;

void unpack(int neuron, double base, double scale, const char *data) {
    for (int i = 0; i < 768; i++) {
        int piece = i / 64 % 6 + 1;
        int color = i / 384 ? BLACK : WHITE;
        int rank = (i / 8 % 8) * 10;
        int file = i % 8;
        FT[piece | color][rank + file][neuron] = (data[i] - ' ') * scale + base;
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
    unpack(0, -69.43, 1.697, "IIIIIIIIIIIII.\"IIIIIII 5IIIIIIIIIIIIII IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIOIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII]IQeIIIIXWY\\\\III]VURRIIIVIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII~I~IIIIIyIIIIIIIIIIIIIIIIII_IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIsIIIIIeIiYIIWWIIVIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII");
    unpack(1, 0.0, 2.546, "                                                                                                                                                                                                                                                                                                                                                                                                                                                 ~                                                                                                                                      Z                                                                ?                                                                                                                                      ");
    unpack(2, -112.7, 1.99, "YYYYYYYYSROYYYYYQDRYYYYYPKPYYOPYEED;Y=YYYY.*,,YY YYYYCEYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYchaYYYYYYcbbYYYYYYadYYYYYYYaYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY|~YYYYYYYjYYYdYlYYY`YeYhY`eaYhYgYYY`YjYiYYcYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYcYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY");
    unpack(4, -260.3, 5.372, "PPPPPPPPPPPPPPPPPPONPPPPPPOOPPPPPPPPPPPNPPPPPPPPPBGPPPPFPPPPPPPPFFFFFFGFDCEEEFEEFEEEEFEFCDDDDDDDCCCCECDCABACBCCBB@ABCBPPBP;PP;PPCDDDCDDDBBCCCCDBBCBCCDCDBBBBCCCBBCBBCBCBAAAAAABABBA@@B@C?999P99B??>>>?????>???@@???>???@??=>??>?===<?>==<<;<===<<=<<>=====><>A@?$%%$%%%%$$$%%%%%%%%%&&%%$%%%%&%%%%%%$$%#% \"$$#\"\"$$$('#$$\"'34/($\"PPPPPPPPPPPPPPPPPPPPPRPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP_^PPPPXPPPPVTPSPPTQRPPSPRTTTPPRPSSVSPPPPPRSRPPPPPPPPPPdPffPe`VVXZ[ZfeVYY\\[\\[XYZ[[Z[[ZYZZ[[[[ZZZ[[[\\[ZWPZZZ[ZYYZZZZZZYVPPPhPPYYZZYXZYZYZZ[[P\\ZY[Z\\\\\\[ZZZ[[]\\[ZZ[[\\[]\\\\ZZ[Z[[\\YY[Z[Z[ZZbbcadfecabbbdccbaa`bccbaabbaccbb`baabba`_aaabba`^``abaa___``a```jq}~ysnkllmrrmnmljlnoomllmjnnnmllmmmnnmmllmmmmmmkklllllljkklkkkjPPTPPPPPPUPTPPPPPTTSPTPPPRPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP");
    unpack(5, -32.52, 2.728, ",,,,,,,,,,,,\",,,,,,&$,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,;,,,,,,,,, ,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,DF,>,8=,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,K,,,,~,cEK,,,O,]A?,,,=,VA,,,,7,R:,,,,,,]D==;470,,,,,,,,O,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,F,,,,,,,F,,,,,,,H,,,,,,,I?6,,54,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,<,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
    unpack(6, -168.8, 5.576, ">>>>>>>>>>;>>>>>>>>>>>>>>>>>>>>>>>>>>>>9>>>>>>>3>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>A>G>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>H>I>>~DG>>HL>[>>>>>J>W>>>A>H>V>>>C>H>W>>>D>GAZ>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>I>>>>>>>K>>>>>>>O>>>B>BFP>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>H>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>C>>>>>");
    unpack(7, 0.0, 2.754, "         B j                                         ~                                                                                                                                                                                                                                                                                6                                             S`                                                                                                                                                                                                                                                                                                                                                                                                          ");

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
