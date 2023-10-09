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

const char *AMPLITUDES = "e=HYN_W9deZAJFP.?bV]TMMkJCJZZ^Y0Ja\\KYFUb?BUgX\\M<`a\\CNKRNDLUbXML?4SMHYTD_oZBaNL-W@FeFV^tK_bA]^C5]KDeHM_gX`^D^TGFUDO]ICbVY]MQ^^EFCOHNJXa^\\JXUWK2[iVPUMalFAXKLYD9^YIbZP]^OLYUIKIGHWLYVJMM[LNLMRJ;C]G\\SV0c\\NWQG[e\\LWXN]McHFWLPLMK]ZK^LYUPVQI^T\\M^I]ZMKG\\QXXKWQYJ?EYGYYL<daH[MG]KZ?WG]UOZJaXZIYME\\BKXYIYEXcYZN[X`IFSMVGLE\\]NUXZPYoGCGIZ=cfcL_FV~;e=gE^e\\H=aHXHGBacCHNac>X;T`]@DaCaaICO_DbGC^\\GNB^KFYIY?d^H`GWJWcF[LYUNd@DXJ]WZB\\XP]IXN_L]]FZJ[@HLNcN^R_]P\\HXNNCGLNULQ2^JMZUXN*OINYYZUjO^YKLIN `cZLMXPqEKQNXYY)c\\ZVJHTgGMNWX]Z]5L@[HYJE[ZgKaM3TEVCVLLHO`ScV_YJLB]C[QZHPZP_O\\UFPENFYSUYKL[ZMKW\\@ZFXJM>LI[WMKM@L]ZUUMKCL]ZLOJOZVMZLNIJHXIKJNKP[X\\ZWL^OVNKOYRJOfMJMNVN;MJVPYZMMIMWOVNWN\\\\VTWWY_LVWKXONZUL[KOLQMTNVTUYNYWMXNXNWF9HSIPX,gc\\ZYOI4ejMbG\\Y^FF[Z\\MYVXdLZXOJcJEHUOLOGPa[G[_[`JHQJLJ]X2X>ZCNDOdHfMeN/^G^@]EKkWfNMW]O>TEXK[JMdZbPLK^WGQIMM]LL\\M[WJKXWgjIGKMV5=>ZY\\KO5HC]XPWWqbeLQK\\X=B>P_YPNbgaLJJNOV@ELQ\\ZMI`YQMKJO";

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
            double amplitude;
            if (rank1 || file1) {
                amplitude = (*AMPLITUDES++ - ' ') * 0.15179560256823169 - 7.664392110109058;
                amplitude = copysign(amplitude * amplitude, amplitude);
            } else {
                amplitude = matvalue;
            }
            for (int rank2 = 0; rank2 < 8; rank2++) {
                for (int file2 = 0; file2 < 8; file2++) {
                    int v = round(
                        amplitude *
                        cos((2 * file2 + 1) * file1 * 0.19634954084936207) *
                        cos((2 * rank2 + 1) * rank1 * 0.19634954084936207)
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
