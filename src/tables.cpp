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
int SKIP = 0;

const char *AMPLITUDES = "-KY--I5CD!GUDDC.\"K.3D3U5'EL#5L\"C[P.5I-6D;3[9-\"93D[I\"DUGLO3.DU\\D!O9YDIZI._3UL-L-_C$-CALY5\"L[-\\D-LDP.KL-LF$-K\"-KADLGDLZ_I9GD33L.UZ(J-\"-UI'-Z-DACLL-I3OCL\"'[KZ[3#D-KAZD\"I";
const char *SKIPS = " %  !#  %!%!%  !##$\"#&'''!%' &' '!#!!#\" \" \" #)# )!'!!#'%!<%'!$ !$ #<%''G % &'G&W& (/'6%  %  !-%).!#   $    \"\"%   \"  !# ! \"\" !%!!!!!!!##!%'!5& '&  %/#+ %  %!%  %  % '";

double AMP_PALETTE[64];
double AMP_1[] = {-3.66485687, -0.20226109,  0.90070843,  1.20320102,  1.53022837,  1.87065501,
  0.67319052,  3.62225496};
double AMP_2[] = {26.94983537,  -4.12369304,  -2.59360384,  -5.71396615,   3.35529018,
 -38.32784466,  28.54130973,  -3.54970135};

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

void extract(int piece, int phase, int colmul, int matvalue) {
    for (int rank1 = 0; rank1 < 8; rank1++) {
        for (int file1 = 0; file1 < 8; file1++) {
            double amplitude = rank1 || file1
                ? SKIP--
                    ? 0
                    : (SKIP = *SKIPS++ - ' ', AMP_PALETTE[*AMPLITUDES++ - ' '])
                : matvalue;
            for (int rank2 = 0; rank2 < 8; rank2++) {
                for (int file2 = 0; file2 < 8; file2++) {
                    int v = round(
                        amplitude *
                        cos((2 * file2 + 1) * file1 * 0.19634) *
                        cos((2 * rank2 + 1) * rank1 * 0.19634)
                    );
                    PST[piece | WHITE][rank2*10+file2] += phase * v;
                    PST[piece | BLACK][70-rank2*10+file2] += phase * colmul * v;
                }
            }
        }
    }
}

void init_tables() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            AMP_PALETTE[i*8+j] = AMP_1[i] * AMP_2[j];
        }
    }

    extract(PAWN, 1, 1, 51);
    extract(PAWN, 0x10000, 1, 91);
    extract(KNIGHT, 1, -1, 254);
    extract(KNIGHT, 0x10000, -1, 312);
    extract(BISHOP, 1, -1, 260);
    extract(BISHOP, 0x10000, -1, 348);
    extract(ROOK, 1, -1, 333);
    extract(ROOK, 0x10000, -1, 583);
    extract(QUEEN, 1, -1, 630);
    extract(QUEEN, 0x10000, -1, 1190);
    extract(KING, 1, -1, -15);
    extract(KING, 0x10000, -1, 0);
    extract(PASSED_PAWN, 1, 1, 7);
    extract(PASSED_PAWN, 0x10000, 1, 26);
    
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
