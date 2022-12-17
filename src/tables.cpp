#define NEURONS 6
int16_t FT[25][SQUARE_SPAN][NEURONS];
int16_t BIAS[NEURONS] = {-128, 13, 46, 58, 103, 118};
int OUT_W[2*NEURONS] = {-58, -71, -38, -127, -56, 154, 59, 70, 36, 135, 57, -158};
#define OUT_B 1017

void unpack(int neuron, double base, double scale, const char *data) {
    for (int i = 0; i < 768; i++) {
        int piece = i / 64 % 6 + 1;
        int color = i / 384 ? BLACK : WHITE;
        int rank = (i / 8 % 8) * 10;
        int file = i % 8;
        FT[piece | color][rank + file][neuron] = (data[i] - ' ') * scale + base;
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
unpack(0, -163.0, 3.661, "MMMMMMMMMMHMMHGJMMMMMMMKMMMMMMMMMMMMMMMMAC<H:MMM6M937MMMMMMMMMMMVMMMMMMVMMMMMMMMMMMMMMMMMMMMMMMMMMMMHMHMMMMM=BMMMMMMM>M>MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM@MMMMMMMMMMMMMMMMZVVVVVV\\VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVMVVVVVVVVVVVVVVVVVVVMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMFMMMMMMMMMMMMMMMMMMMMMMMMEMSMM=9MMMMMFCBMMMMMMMMMMMMMMMMMMMMMMMMMMV\\MMMMMipnMMMMMMMMMMMMSSSSSSSSSSbemiojHSSS^c`\\LPSSSWSSJSSSPSSSMSSSSSSSMSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSXS[XSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPVMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMoooooooootoooooooooooooouoooooooooooooooooooooooooooooooooooooooMMMMMMpMMMMupz~lMMMzmnmkMMMMb`^MMMMMMMMMMM6  =MMMME78DMMMMMBGHMM");
unpack(1, -376.3, 5.15, "hhhhhhhhhhhhhhfchhhhhhfdhhhhhhdchhhhhhcakhhhh^ZXhnhhh\" )hhhhhhhhccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccbbbbbbbbbbbbbbbbbbbbbbbbbbbbb`bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb[bbb```````]```````````````````````Z````````````````````````````````OOOOOOOOOOOOOOOOKOOOOOOOOOOOOOOOHHJJOOOOFOAOOOOODOGOOOOOFOOOOOOOxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxoooooooooooooooooooooooooooooookoooooolhoqooooggoroooiegoooooooommmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmppmmmmmmommmmmmmmmmmmmmmmmmmmmlllllllllllllllllllllllllllllllllllllllllllnnllllnllllllllkllllloooooooooooooooooooooooooooooooooooooooooooooooooooooopooooooptrxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx~xxxxxxxxxxx~xxxxxx||xxxxxxxxiiiiiiiiiiiiiiiiirqiiiiiiiisiiiiiilmiiibiiiiiib\\tiniia[Vrrqic]VU");
unpack(2, -653.1, 10.87, "YYYYYYYYYXYYYWWYYYXYYYVYYYYYYYYYYYY\\\\[Y[YYba_]YYYYY_YY]YYYYYYYYYMMMMMMMMMMMMMMJMLMLMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMlMMMMMMMPPPPPPPPPPPPNPNPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP???????=????;???????????????????????????????????????????????????```_````````````````````````````````````````````````````````````\\\\\\\\Y\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\& \\\\\\\\\\\\\\\\\\\\\\\\\\________R_______________________________________\\_[[\\]]\\________lllllsllllllllllllllllllllllllllllllllllllllllllllllllllllllllllmmmmmmmmmqmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmpmmmmmmnmmmmmmm~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\\\\\\\\\\\\\\\\\\`\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_\\\\\\\\\\\\\\\\\\\\\\\\\\\\gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg");
unpack(3, -234.7, 4.689, "KKKKKKKKKKKKKKKKMMMKMKMKLKLMMKKKKKKMKKKKGFGKDKKKDB>;9A=@KKKKKKKKMLGGGGLGLGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE;;;;;;;;;;;;;;;;;;;;;;;;85;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;$$$$$$$$$$$$$$$$$$$$$$$$$$$ $$$$$$$!!$$$$$$$$$$$$$$$$$$$$$$$$$$$bbbbbbdebbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbVVVVVVVVjka`[][[_^]V[Y\\]YWVVSVVXVVTSSSVVVVTTTSVVVVVVVVWWVVVVVVVVYYYYYYYYYYYYYYYYYYYYYYYYYYYY[Y\\YYYYY[YYYVYYYYYYYYYYYYYYYYTVYYYYR\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\Zddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd`{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{~{{{{{{{{{{{{{{{{{{{{{{u{{{tRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRORRRRRNMMRRR");
unpack(4, -245.3, 8.077, ";;;;;;;;<==97:;;=;;977;;;;;;88;;;;;;;;;;;;;;;;;>;IE;;;;;;;;;;;;;565555555555555555555355555555555555555555555555555555555555555F66866666666666666666566666666666666666666666666666666666666666661011131311111111111111111111111111111111111111111111111111111111%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%PLIBA>>AMHD>>>>>IC@>>>>>E>>>>>>>>>>>>>>>>>>>>>>>[Q>>>>>>TR>>>>>>>>>>>>>>uxs>>>>>CDE>>>>>CBC>>>>>BBB>>>>>BBA>>>>>CBB?>>>>>>>>>>>>IIQQIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIGIIIIIIIIIIIIIIIIIIHMHHHHMHHHHHHHHHHHHHHHHLHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHJHHHHHTMMMMMMMRMMMMMMMQMMMMMMMMMMMMMMMPMMMMMMMMMMMMMMMMMMMMMMMOMMMKLMMibbbbbljbgbbbjmibbbbbiiibbbbbbebbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbLLLLLLxLLLLLmt~LLLLLpvtLLLLLLTSLLLLLONLLLLLLLLLLBFHLLLLLDDGLLLLL");
unpack(5, -187.4, 4.203, "MMMMMMMMMMMMMOQMMMMMMMQMMMMOPNOMMONMMPOMMMMMMOMMMMMMMMMMMMMMMMMMSSSSSSSSSSUUUVSSSVVXXWWTVXYYYZYWWX[[Z[WYW[]^]]^XSWZ\\\\ZWSSSSSYSSSXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXWWZZ[[WWWWYYWYWWWYYYYYYWWW[[[[ZW\\]^^^W^]\\__`_``_WWWbba``[`aaaaaaiiiiiiiiiijjjiiiijkkkkkijklilmllikmnoomniiipqrpmiioiiqnniopqiromMMMMMMMMMMMMMMMMMMMMMKMMMMMMMMMMMRMMMMMMRSVMMMMMYVMYMMMM\\ZYZXMMMJJJJJJJJJJJJJQROJJJAJJJMJJJEEJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA?????????<;:;:<?=999888?=;9789:<<;:99;;<=;;;<;;<<=<==<<=????????<<<<<<<<7<2<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<((((((((&((! (((((#(((((((((((((((((((((((((((((((((((((((((((((mmmmmy~~mmmmmxm~mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");

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
