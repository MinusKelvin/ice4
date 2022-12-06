int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

void unpack(double base, double scale, const char *data) {
    for (int r = 0; r < 16; r++) {
        for (int f = 0; f < 48; f++) {
            double r_value = data[r+48] * scale + base;
            double f_value = data[f] * scale + base;
            int rank = (r % 8) * 10;
            int file = f % 8;
            int phase = r / 8;
            int piece = f / 8 + 1;
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
        for (int piece = PAWN; piece <= KING; piece++) {
            for (int sq = 1; sq < SQUARE_SPAN; sq++) {
                PST[phase][piece | WHITE][sq] = material[phase][piece-1];
                PST[phase][piece | BLACK][sq] = -material[phase][piece-1];
            }
        }
    }
    unpack(-0.9901, 0.1884, "HABI@=NB2SG1YQ8h_>f[=?Ui9/2E4AH:QN?`VDn`ByuC~GBbGOPQRQQIQ[_aa_]R&#%%'%'()\" *!!(%\"$\"()*')*$*! %*%*'&!$!% ) ($ #&(')&(\"*(\"$!#'\"*&%"); // mse = 812.9
    unpack(-12.31, 0.2317, "^Jb`D\\aCUcDUdFVeHS[OWX_eXUT^QVdPSgQNdRMTSQ8UQ_QN$#$%'#\" }{~|zzwzSWVTUSSRRYWXWRRTXWXWSRTTUQSRRWQUVXURSUUSRVYYXUVTSRWUUQUWQWUUUQWR"); // mse = 209.9

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
