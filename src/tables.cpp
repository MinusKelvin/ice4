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

const char *AMPLITUDES = "&F 0.7/M&X0' 6(^=P(,(-.1 5 00H0! P+-06(P=>(%/,.N7P,>.-(.5-(P/.-5*-.(-(#H -0(/.]. .000(3.H0-- .J7?0-./(;6 (./00]?)0( *(%*-.//@0=U(-*0(5HW0>P.-:/'6&6(H<-HP>@H52,-5X*-7%/PH5@(*6(5.@ 5P(0@-(HH6@2-'8*0 680%-P-#(6(5(--*.7(?(H0 ->@58(8 (0(7.+(*(6.*0((0--80--/*5.*. /PH) /(/-#8O(((-P46>/--05MH0 P0(,H.-0( - **/-0( .-8-.--( +=0*/ .F- 8/.--'-,0((-->-@0-. .0(-0-.   / - .-(8/,0/-@.(.-.0( .E5@*+((!?)./(*8X,-//.@-?*6/-(-.-,0-H-0(((( @(+-H @0.-*+(//-/(0 X- -.(.E. ((00.- -/.(.(.++(((/07-(/-/..8(-8-.-(.(.(((0.0/./././=60*00-N&P 8-*@-0=/*@(.0 P/0 0-6)>-/0 06/?00.8/7 6(-(*-6+@.(/8(6M ( (/:%?+00. U&O-P*+0H*680)-0(/&-0/. ? 6 (.-.*(P8*8787  ( - @0W*5* 0F?X?-76(LE&NG6HX) FP*/**>P?5*.P?F/E(7,'5P5PP 5.75?*>H+*./#/F05.5.&*X.X.^H*H',6 3/X.-/,.F(6/ 8 -&0P(- H/*( -.,--,-8( -//>H-*/ 0F?@ 7*/ /?*8-0(.&=5/ @/0>)/(, /.7-@,60 8= -.?.H(7,()*/..G3 *--(2NF80,-.2 5@/(//;PX-(-)/N>F(H0(.P%P-  ..(=6-(+0- 70(-- .";
const char *SKIPS = "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               ";

double AMP_PALETTE[64];
double AMP_1[] = {-3.60988223,  0.29116307,  1.94509165,  3.15035446,  4.79422138,  5.60808756,
  8.57737211, 14.65253513};
double AMP_2[] = {0.56063164,   7.41979564, -10.44222609,   6.73879694,   7.89121979,
  -2.61550805,  -2.01795697,   2.07446933};

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
                    : SKIP = *SKIPS++ - ' ', AMP_PALETTE[*AMPLITUDES++ - ' ']
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
