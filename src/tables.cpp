int PST[25][SQUARE_SPAN];

const char *DATA_STRING = "  '$$!#&BIFBCMNBDJGEGHODEJIJKJLDKOLMNNPHYb][[]`T}~xsqusu%!'$\"\"%!0=7 (#<8540,--12,77:976+9DGHFEB6L[ZXXZYJ\\snhjqs^]|tkkt~_6hi`acr6*-,.*$' 2473542236967:;858;>BA=?89EIROFJ;KMY\\g^T;;PUX\\TN:NTUW]WK0:@DF?8-*5<=<83 2<>>=;;/AHLKJIF?RXZ[ZZWP\\bcfgec[beknlhba_`acca__(+#&## %898044:.3??@;>;35;CHH?;63<ENMJA94FFMMQOC/<=>??>0'388;21&PTWXYUTMRX^_`aZWXadjiedZaglmonjfeisvtvokcru|~{ygZbmsprb_ XadgZY4";

void unpack_full(int piece, double scale, int offset) {
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v;
            PST[piece | BLACK][70-rank+file] += -v;
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
    unpack_full(PAWN, 2.216, -8); // average: 80
    unpack_full(KING, 1.093, -39); // average: 9
    unpack_full(QUEEN, 1.0, 895); // average: 928
    unpack_full(ROOK, 1.0, 416); // average: 462
    unpack_full(BISHOP, 1.0, 286); // average: 312
    unpack_full(KNIGHT, 1.642, 171); // average: 281
    
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
