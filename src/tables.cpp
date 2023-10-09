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

const char *AMPLITUDES = "e=_9deAF.?bkC0aFb?Bg<`aCDb?4D_oBa-@FeFtbAC5De_g`DFDCbECa2ialFAD9b;C0cecF?E<da?aEBEc`FEoC=cfc_F~;e=gEe=aBacCac>;`@DaCaaC_DbCB?d`cd@DB_F@c_C2*j `cqE)cg5@Ega3EC`c_BC_E@>@Cf;_9,gc4ejbdcEa_`2>CDdfe/@Ekf>Edbgj5=>5Cqbe=B>bga@E`";
const char *SKIPS = " #!  !!!  %!%!#!  !#  !$\"# %  ! \"!   \"\" \" \" \" !!#!# '!%'!$   $ #; % &'!E %  ''%!%!%!%(!#   ! !     !\" $   \"  !!!     !!  !$& \"$&  %'####'&'''  % & &(!$\"!!!!%!!#!'%.%''G&7@%  %  !-%!'#!(!!!!!!!#!!!%!%!5 %  %!%  %  %  & &";

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
            double amplitude = 0;
            if (rank1 || file1) {
                if (!SKIP--) {
                    amplitude = (*AMPLITUDES++ - ' ') * 0.15179 - 7.6643;
                    amplitude = copysign(amplitude * amplitude, amplitude);
                    SKIP = *SKIPS++ - ' ';
                }
            } else {
                amplitude = matvalue;
            }
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
    extract(PAWN, 1, 1, 51);
    extract(KNIGHT, 1, -1, 254);
    extract(BISHOP, 1, -1, 260);
    extract(ROOK, 1, -1, 333);
    extract(QUEEN, 1, -1, 630);
    extract(KING, 1, -1, -15);
    extract(PASSED_PAWN, 1, 1, 7);
    extract(PAWN, 0x10000, 1, 91);
    extract(KNIGHT, 0x10000, -1, 312);
    extract(BISHOP, 0x10000, -1, 348);
    extract(ROOK, 0x10000, -1, 583);
    extract(QUEEN, 0x10000, -1, 1190);
    extract(KING, 0x10000, -1, 0);
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
