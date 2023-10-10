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

const char *AMPLITUDES = "594.55:&?:36 ><3&3: 5943  3 &->/6>043(&/35&,:7 4( 4?:&5&>(43:,> @ 5454> >&&  3,>> @:&53-  434: 4&&&& 3>-636&: .>3>&:&9: ,9 &<3&53&>>>,&594: &953&:&3&: 34&& &.?537563&>&&53&&3&44(& &93534&=9595&>593& 33 33994: 4 33 4 3& >&-9  5&55?>&>:&659&53>&:>&:3,4&3&5: :4,&:3>456&@99@ (359:94354: 4";
const char *SKIPS = " #!  !!!  %!#!!#!  !#  !$\"# % &'!%  % & & $! %  ! \"!   ! !      \" \" !   ! !# \"\"  !!$\"!!!!%!!#!'#!!+ % &!$   $  \"#'0!#''=) !# $!'!)'#!#)&7? !#  ##!%%!%!%!#!' %  %  ! \"  '%!%!!!!(   \"   ! !       \" #    \"  !!!     !!     !\"!!!!  !!    !!!%!%!#!/ !#  ! # %  %'!!##!!' !#  %!%  %  !#  & &";

double AMP_PALETTE[36];
double AMP_1[] = {-11.469, -7.372, 5.6327, -1.2777, 2.567, 7.233};
double AMP_2[] = {0.4494, -4.3098, -2.8459, -6.644, -10.439, 16.965};

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
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            AMP_PALETTE[i*6+j] = AMP_1[i] * AMP_2[j];
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
