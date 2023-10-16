#define S(a, b) (a + (b * 0x10000))
int PST[25][SQUARE_SPAN];
#define BISHOP_PAIR S(50, 43)
int32_t DOUBLED_PAWN[] = {S(2, 32), S(-21, 32), S(11, 31), S(6, 19), S(11, 12), S(12, 28), S(-17, 30), S(11, 36)};
#define TEMPO S(15, 12)
#define ISOLATED_PAWN S(13, 4)
int32_t PROTECTED_PAWN[] = {0, S(13, 10), S(15, 8)};
#define ROOK_OPEN S(45, 25)
#define ROOK_SEMIOPEN S(23, 18)
int32_t PAWN_SHIELD[] = {S(20, -30), S(29, -45), S(31, -37), S(39, -34)};
#define KING_OPEN S(-55, 0)
#define KING_SEMIOPEN S(-15, 17)

int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

const char *DATA_STRING = ">SU+#5/ AM<707+'AAPQKA3*JUc]TJD5ga~kjeL?7%Xvl_8G(-7SH;7< !310**,#2(\"#,3.29.*,<:;BOEMaOMI^jYUOYn^1=/A/2/=5*/'+%0;1., *,:E8?D9>EKH+2:EDGTVeX~Zfdne/-+ ,.4*3:02-18/KIBB96=@\\WPC>:ISspcF;Obh~{se_dhueJJkA!\"9O1( D*%~$<7 ;`Y7Xzz\\PsvJ1+ '9<<:A;:59;69 5IX->ZY7bqvGZt~15;C ,63*<3,+.,/\"$(\"))'& !$'\")-&0=# ;IH9<KHI8;KX .%.)/496@KO.LNU RXWZ]kg]st~jj~} 5GKAMP^EZhsTjz~";

void unpack_full(int phase, int piece, double scale, int offset) {
    for (int rank = 10; rank < 70; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | BLACK][70-rank+file] += v * phase;
        }
    }
}

void unpack_smol(int phase, int piece, double scale, int offset) {
    for (int rank = 0; rank < 80; rank+=20) {
        for (int file = 0; file < 8; file+=2) {
            int v = (*DATA_STRING++ - ' ') * scale + offset;
            PST[piece | WHITE][rank+file] += v * phase;
            PST[piece | WHITE][rank+file+1] += v * phase;
            PST[piece | WHITE][rank+file+10] += v * phase;
            PST[piece | WHITE][rank+file+11] += v * phase;
            PST[piece | BLACK][70-rank+file] += -v * phase;
            PST[piece | BLACK][71-rank+file] += -v * phase;
            PST[piece | BLACK][60-rank+file] += -v * phase;
            PST[piece | BLACK][61-rank+file] += -v * phase;
        }
    }
}

void unpack_half(
    int phase, int piece, double scale, int qll, int qlr, int qrl, int qrr
) {
    for (int rank = 0; rank < 40; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*DATA_STRING++ - ' ') * scale;
            PST[piece | WHITE][rank+file] += (v + qll) * phase;
            PST[piece | WHITE][7+rank-file] += (v + qrl) * phase;
            PST[piece | WHITE][70-rank+file] += (v + qlr) * phase;
            PST[piece | WHITE][77-rank-file] += (v + qrr) * phase;
            PST[piece | BLACK][rank+file] += (-v - qlr) * phase;
            PST[piece | BLACK][7+rank-file] += (-v - qrr) * phase;
            PST[piece | BLACK][70-rank+file] += (-v - qll) * phase;
            PST[piece | BLACK][77-rank-file] += (-v - qrl) * phase;
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
    unpack_full(1, PAWN, 1.273, 34); // average: 84
    unpack_full(0x10000, PAWN, 1.0, 90); // average: 118
    unpack_full(1, PASSED_PAWN, 1.487, -28); // average: 19
    unpack_full(0x10000, PASSED_PAWN, 1.703, -20); // average: 50
    unpack_smol(1, KING, 1.406, -66); // average: -21
    unpack_smol(0x10000, KING, 1.0, -36); // average: 13
    unpack_half(1, QUEEN, 1.0, 556, 523, 547, 559); // average: 578
    unpack_half(0x10000, QUEEN, 1.153, 795, 983, 882, 979); // average: 853
    unpack_half(1, ROOK, 1.0, 312, 339, 323, 362); // average: 329
    unpack_half(0x10000, ROOK, 1.0, 504, 546, 504, 533); // average: 510
    unpack_half(1, BISHOP, 1.0, 244, 257, 245, 266); // average: 274
    unpack_half(0x10000, BISHOP, 1.0, 284, 298, 293, 293); // average: 310
    unpack_half(1, KNIGHT, 1.342, 168, 200, 178, 211); // average: 259
    unpack_half(0x10000, KNIGHT, 1.113, 220, 232, 222, 229); // average: 279
    
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
