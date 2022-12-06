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
            PST[phase][piece | WHITE][rank + file] -= r_value * f_value;
            PST[phase][piece | BLACK][70 - rank + file] += r_value * f_value;
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
    unpack(-24.94, 0.3719, "`omlkoSXdegfeum~\\cb_eriSXXVTX]WYCDB>=G<I/%  )-=QWPPPOJJTyyzzyzxz");
    unpack(-36.31, 0.4788, "jadffduVvttuxtvY{xx{ysu|qoprtvz~}}{z{v{y=<650, *PUUTUVXRoqommnpm");
    unpack(-45.68, 0.731, "QVX[[[_ZQOOMNNNFMKLNNLLOEECDDEFF(&$#\"  $VSQPPPMIkljhhmnmxy{~~|zy");
    unpack(-24.42, 0.5732, "COPSTZa8YZ\\Z\\_\\UXX[]__\\[eecdgiiiyxxzzzx~GCAADHIEA?;77>@A  $))%!#");

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
