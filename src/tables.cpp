int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int STARTS[] = {0,0,8,4,0,0,0};
int ENDS[] = {0,0,16,8,4,8,8};
int SLIDER[] = {ROOK, ROOK, ROOK, ROOK, BISHOP, BISHOP, BISHOP, BISHOP, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT, KNIGHT};
int LAYOUT[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
int RAYS[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
int DELTAS[] = {640, 148, 416, 509, 749, 1348, 0};

double LOG[256];

#define HIDDEN 32
#define QA 255
#define QB 64
struct RawNet {
    float p1[12][HIDDEN];
    float p2[12][HIDDEN];
    float f1[8][HIDDEN];
    float f2[8][HIDDEN];
    float r1[8][HIDDEN];
    float r2[8][HIDDEN];
    float bias[HIDDEN];
    float h_mg[2][HIDDEN];
    float h_eg[2][HIDDEN];
    float h_bias_mg;
    float h_bias_eg;
};

int FT[25][SQUARE_SPAN][HIDDEN];
int BIAS[HIDDEN];
int HL_MG[2][HIDDEN];
int HL_MG_B;
int HL_EG[2][HIDDEN];
int HL_EG_B;
int FLIP[SQUARE_SPAN];

#ifdef OPENBENCH
// Deterministic PRNG for openbench build consistency
uint64_t RNG_STATE = 0xcafef00dd15ea5e5;
uint32_t rng_32() {
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

uint64_t ZOBRIST[25][SQUARE_SPAN];

void init_tables() {
    RawNet raw;
    memcpy(&raw, net_data, sizeof(RawNet));
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            FLIP[rank*10+file+21] = file+91-rank*10;
            for (int piece = PAWN; piece <= KING; piece++) {
                for (int i = 0; i < HIDDEN; i++) {
                    FT[piece | WHITE][rank*10+file+21][i] = (
                        raw.p1[piece-1][i] * raw.f1[file][i] +
                        raw.p2[piece-1][i] * raw.r1[rank][i] +
                        raw.f2[file][i] * raw.r2[rank][i]
                    ) * QA;
                    FT[piece | BLACK][rank*10+file+21][i] = (
                        raw.p1[piece+5][i] * raw.f1[file][i] +
                        raw.p2[piece+5][i] * raw.r1[rank][i] +
                        raw.f2[file][i] * raw.r2[rank][i]
                    ) * QA;
                }
            }
        }
    }
    for (int i = 0; i < HIDDEN; i++) {
        BIAS[i] = raw.bias[i] * QA;
        HL_MG[0][i] = raw.h_mg[0][i] * QB;
        HL_MG[1][i] = raw.h_mg[1][i] * QB;
        HL_EG[0][i] = raw.h_eg[0][i] * QB;
        HL_EG[1][i] = raw.h_eg[1][i] * QB;
    }
    HL_MG_B = raw.h_bias_mg * QA * QB;
    HL_EG_B = raw.h_bias_eg * QA * QB;

    for (int i = 1; i < 256; i++) {
        LOG[i] = log(i);
    }

    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST[i][j] = rng();
        }
    }
#else
    fread(&ZOBRIST, sizeof(ZOBRIST), 1, fopen("/dev/urandom", "r"));
#endif
}
