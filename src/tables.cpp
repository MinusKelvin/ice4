int16_t FT[25][SQUARE_SPAN][13];
int16_t BIAS[13] = {270, 301, -90, 89, 151, 122, 88, 112, 63, 113, 27, 117, 0};
int OUT_W[24] = {-78, -141, -78, 32, -18, -105, 128, -38, -122, 50, -76, -71, 79, 122, 80, -32, 17, 131, -141, 37, 122, -50, 76, 75};
#define OUT_B 3366
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

void unpack(int neuron, double base, double scale, const char *data) {
    for (int r = 0; r < 48; r++) {
        for (int f = 0; f < 16; f++) {
            double r_value = (data[r] - ' ') * scale + base;
            double f_value = (data[f+48] - ' ') * scale + base;
            int rank = (f % 8) * 10;
            int file = r % 8;
            int color = f / 8 ? BLACK : WHITE;
            int piece = r / 8 + 1;
            FT[piece | color][rank + file][neuron] = r_value * f_value;
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

uint64_t ZOBRIST_PIECES[25][SQUARE_SPAN];
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;

void init_tables() {
    // Feature Transformer
    unpack(0, -15.06, 0.4783, "CDCDDCCDNOOOOONMOPQQQQPO^^^]]]\\]|~~~~~~~Z[\\^^\\ZY !!!!!! WXWWWVUT");
    unpack(1, -8.115, 0.2677, "<;=DSay~68>ACDDG8>@CFGIJ??>BACCC@EHFEA@<AA@GWdnv@<:2+% &?CBAELR[");
    unpack(2, -3.403, 0.2318, "/0001320adhjjlhfJSUYYWTMPSUWVXUPnux~~zysQSPNNNRO,*('# %#DIKJHDDC");
    unpack(3, -12.41, 0.3521, "EDGLMHFEDDFGGFEE#\" $%#%%CDB@@ACBAAAA@@??{{|}|}~~A@BBA>>=pkifffgf");
    unpack(4, -19.26, 1.224, "22222231:998888888888888<;;::::;MLKKKJJJ'(())*)) !\"\"%y~}><<=>>??");
    unpack(5, -5.754, 0.188, "e~ul]X=KRNRPNL@FBBDFF@:>`[[ZZ]\\l?FLQROF@?TbqqL< 8:951,%![^TJDAAH");
    unpack(6, -7.767, 0.3677, ";A@>=;?;DGHHHGGDLMLLKKMKJMNNONOK}}~}~~~}KIE@=@BBGHIJKKKF $%%$##$");
    unpack(7, -14.85, 0.4036, "KLJJJIJL``___^__baaaaa`b~~~~}}~}vvuuuuvx::;99876   \"$$''nkmmlklm");
    unpack(8, -31.34, 0.3986, "J3?Xhvtpeedgkprvhbchjmnrromlnlmmytpoosw| #+EWlmpfijlnopl~~zwtppo");
    unpack(9, -6.135, 1.169, "~%'&&%%%%&&&&&%%%%&&&%%%(''&&&&&''&&&&&&\"$$%%%%%73122/.1+$$%%&& ");
    unpack(10, -22.59, 0.363, "]rrmfZ9Aeagca_[`]b^b_XTaZZ]_aegvOPVXX[^g~zypiR5 _`a_`dl^bhccefho");
    unpack(11, -8.427, 0.3469, "PWbomhTJ+-0000.)663214734789877186554245==8.,067 9:;:56Y~N<9;;>G");
    unpack(12, -88.31, 2.73, "AE?87FGD=;<<=;99DB>:<>A>EFFEEDA:HNNONKI?utsbfbt~'/@LMZ_ C>;66;CI");

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
