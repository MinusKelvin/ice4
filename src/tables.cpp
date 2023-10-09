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

const char *AMPLITUDES = "A7*.3/53(773+.-0%+6252//9.,.4453&.64/3-26+,2834/*664,/.1/,/263//U+'1/.32,5;4,6//$3+-7-25</56+55,'5/,7./58365,52--2,05.,6235/155,U-,0-0.3654.323/&49202/6:-+3./3,(53.640550/32./.--2/32.//4////1.Z),5-412&74/31-474/33/5/7--3/0///54/5/32020.524/5.53//-4033/313.i+,3-33/)76.4/-5/3*3-5204.634.3/-4+/33.3,37340436.-1/2-/,45/2340(3;-,-.4*787/5-2@)7*8-584.*6-3--,67,-/67*3)254+,6,66.,05,6-,54-07+5/-3.3*75.6-3.37-4/32/7+,3.534+4304.3/5/55-4.4+.//70515404-300F,-/02/0&5./3230$0./33429053//./ 674//30;-.10333#7442.-28-/03354X5'/+4-3.-448/6/&2,2,2//-0617253./+5,414.0405042-0-0-3123//44//3[4+4-3./*/.43/./+/5422//,/53/0.042/4/0...3./.0.043443/5020/031.0g8/./020)/.2034//./30202/44222335/22/30/42/4.0/0/202223/33/303/3~-(.1.03$874430.'79/6-435--444/3237/430.7.-.20/0-064-4546..1./.523&3*4,0,07-8/70%5-5+4-.9380/250*2-3.4./7360/.53-0.//5//4/42./32<89.-//2'**434/0'.,53023;67/1/43*+*0530/686/..002+,/144/.630/..0";

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

void extract(int piece, int phase, int colmul) {
    for (int rank1 = 0; rank1 < 8; rank1++) {
        for (int file1 = 0; file1 < 8; file1++) {
            double amplitude = (*AMPLITUDES++ - ' ') * 0.44376810178819875 - 7.664392110109058;
            amplitude = copysign(amplitude * amplitude, amplitude);
            for (int rank2 = 0; rank2 < 8; rank2++) {
                for (int file2 = 0; file2 < 8; file2++) {
                    int v = round(amplitude * cos((2 * file2 + 1) * file1 * 0.19634954084936207) * cos((2 * rank2 + 1) * rank1 * 0.19634954084936207));
                    PST[piece | WHITE][rank2*10+file2] += phase * v;
                    PST[piece | BLACK][70-rank2*10+file2] += phase * colmul * v;
                }
            }
        }
    }
}

void init_tables() {
    extract(PAWN, 1, 1);
    extract(KNIGHT, 1, -1);
    extract(BISHOP, 1, -1);
    extract(ROOK, 1, -1);
    extract(QUEEN, 1, -1);
    extract(KING, 1, -1);
    extract(PASSED_PAWN, 1, 1);
    extract(PAWN, 0x10000, 1);
    extract(KNIGHT, 0x10000, -1);
    extract(BISHOP, 0x10000, -1);
    extract(ROOK, 0x10000, -1);
    extract(QUEEN, 0x10000, -1);
    extract(KING, 0x10000, -1);
    extract(PASSED_PAWN, 0x10000, 1);
    
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
