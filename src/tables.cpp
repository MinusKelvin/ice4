int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

void unpack(double base, double scale, const char *data) {
    for (int r = 0; r < 48; r++) {
        for (int f = 0; f < 16; f++) {
            double r_value = (data[r] - ' ') * scale + base;
            double f_value = (data[f+48] - ' ') * scale + base;
            int rank = (r % 8) * 10;
            int file = f % 8;
            int phase = f / 8;
            int piece = r / 8 + 1;
            PST[phase][piece | WHITE][rank + file] += r_value * f_value;
            PST[phase][piece | BLACK][70 - rank + file] -= r_value * f_value;
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
    int material[][6] = {{25, 135, 177, 267, 671, -68}, {128, 326, 431, 727, 1420, -90}};
    for (int phase : {0, 1}) {
        for (int piece = 0; piece < 6; piece++) {
            for (int sq = 0; sq < SQUARE_SPAN; sq++) {
                PST[phase][piece+1 | WHITE][sq] = material[phase][piece];
                PST[phase][piece+1 | BLACK][sq] = -material[phase][piece];
            }
        }
    }
    unpack(1.166, 0.1453, "%$\"#*@c%:ENV\\`V@-6<>AD=3%\"%+5;?A '1;DIIE375:EOS_dflrrnec_z~}}}|b"); // mse = 912.9
    unpack(-7.766, 0.1679, "ILDEIZ~I7BDFC.4X22541'3PGOPRI:8)TTbr{u|`-Genq{~+.(%(&$ 'jqqnpqus"); // mse = 200.4
    unpack(-11.82, 0.2066, "Zcakq~xZJWUUXmiTRSTS]iWJ]`Y_eqk]laTOKUL}G8CR\\[H CUYdghYMn_VPOPXh"); // mse = 132.1
    unpack(-12.97, 0.3341, "CLJHDA CPMNIGPN~@@EHJLIENMLJIKE@<DKOSUQE==EHF@;0<BDHOGLG>EJLKJE@"); // mse = 84.76
    unpack(-20.7, 0.3137, "eui[a[5ebgiqli] ahi^[fhc\\elihk_hntoup|zqm~|lf\\@=`c`]]giee`beea^e"); // mse = 47.52
    unpack(-8.014, 0.1478, "QQFMIHCQ_^XO`Y~+TORUa[U!NUJIJG^O@HKU`]ka FCRWN[eYJW^JnEMFKPX_RZX"); // mse = 33.78
    unpack(-7.799, 0.1714, "P ,4.3YPAK?L[;cMMADSNBD~D<BPUMl`TVKVSI[UUS:+#8W'MCMLQJF[OKKMMKIQ"); // mse = 27.58
    unpack(-9.322, 0.09898, "S^pa[hOSSE_kU~-[EYbO7]W]DWTGX^8F`DaUOS8Gufrinb Ew}vuxu{uxtxuuzwy"); // mse = 17.33

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
