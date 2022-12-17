#define NEURONS 12
int16_t FT[25][SQUARE_SPAN][NEURONS];
int16_t BIAS[NEURONS] = {7, 140, 290, 192, 124, 52, 256, 98, 92, -49, 84, -9};
int OUT_W[2*NEURONS] = {-32, 86, 36, -57, -62, -55, -63, -99, 32, 102, 138, 114, 32, -89, -37, 63, 61, 58, 60, 100, -32, -90, -134, -100};
#define OUT_B 50

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
    unpack(0, -412.9, 5.368, "mmmmmmmmmmmcmammmmmmm_mmmmm\\m[mmmmm^m_mmmmmmmcmm*mm+m mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmkmmvmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh`hhhhhhhhhhhhhhhmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmqmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmpmqmmmmmmmmmm~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~z~mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmgmmmmmmmmfmmmmmmmmmommmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmqmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm");
unpack(1, -338.2, 4.828, "iiiiiiiiiffiiionhifiiilmiigiiilniiiiiiiliiiiiiiiiiiiiiiiiiiiiiiimmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmimmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmqqqqqqnjqqqqqqqjqqqqqqqnqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq~~~~~~~s~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PSW\\]futVY_cfkqs_bafffffaffffffffffffffffffffffffffvtfffffffffffcccccccc?($ ccccccccccccccccccccccccccccccccccccbccccccccccccccc\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWXWTW???????7?????????????886???????????????????????<????????????????fffffffffffffffffflfffffffffffdfffffffffffffffffffffffffffffffff");
unpack(2, -75.06, 1.836, "IIIIIIIIIIR h9IIIIf)~+VIII`&}.IIIIeItIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIRIWIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIXIIIIIIIIIIIIIIPIIIIIIIIIZIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIT........................................................ .......IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII");
unpack(3, -142.5, 1.863, "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\O\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\mmmmmmmmmmm^]mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV                                                                !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm~mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmvmmmmmmmmmmmmmmmmmmmmmmmmmmmm");
unpack(4, -265.9, 7.0, "@@@@@@@@@@@@@@@E@@@@@@@E@@@@@@@C@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;=;;;;;;;;;;;;;;;;;;;;;;;;;===========;====================================================8888888588888888888888888848888888888888888888888888888888888888\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\" \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"FFFFFFQSFFFFFFFOFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFIIIIIIIIIIIIII|XIIIIIIIIIJIKIILIIIIIIIIKIIIIIIIIIIGIIIIIIIIIIIIISSYSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY[YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY{~ttttttt|ttttttttttttttttttttttttttttttttttttttttttttttttttttttFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFHFFFFFFFFFFFFFFFFFFFFFFFFFFF");
unpack(5, -54.15, 5.369, "*********&$'*13**&****1**(************************************************&*******(***********************************************************0*************************************************--***************************************************************;*5******************************$*****************************\" #**9>A !***6A@*******4*******2*********************************************{~z******r*****************************************,,,,,,,,,,,,,,,,,,,,,,,,,0,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,////////////////////////////////////////////////////////////////<<<<<<<<JC<<<<<<H<L<<<<<GE<<<<<<<D<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<**********;*****************************************************");
unpack(6, -63.83, 1.508, "888888888888+$88888888888888888888888888888888888888888888888888jJJJJJJfJJJJJJJJJJJ=JCJJJJ:5JJJJJJ>JJJJJJJJJJJJJJJJJJJJJJJJJJJJJ????????????????????????????????????????????????????????????????                                                                JJJTJJJJJJJJJJJJJJJJJJJJJJJ9JJJJJJJJ$J,JJJJJJJJJJJJJJJJJJJJJJJJJJJJJdJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJZxJJ`JJJJJJJJJJJJJJJJJJJJJJJJJJJJJVJJPJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ`lJgJJJJJb`JJJJJJJJJJJJJJJJJJJ$JJJJ3JJJJJJJJJJJJJJJJJXJJJJJJJJJJJJJJJJJJJJQJJJJJJJJJJJJJJJJJJJJJJJ@JJddddddddddddddddddddddddddddddddddddddddddddddddddddddddYddddddUJ\\JJJJ~JJJmJJJhJJeJJJJJJJZJJJJJJJJ_\\hJZJJJYJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ");
unpack(7, -482.5, 9.53, "SSSSSSSSSSSSSSSTSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSHHHHHHHHHHHHHHHHHHHHHHHHFHHHHHHHHHHHHHHHHHHHHHHHHFHHHHHHHHHEHEHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHFHHHHHHHHHHHHHH<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ $$$$$$$$ SSSSSSSSSSSSTSSSSSSSSSSSSSSSSSSSSPNNOSSSSHEHLLMSHHHJKHHSSHDKKMSSSSSSSSSSSPSSOSNSSSSQSSSSSTSSSSSSSSSSSSSSSSSSSSTSSSSSSTTSSSSSSSSSR]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]^]]]]]]]]]]]^]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}~}}}}}}}}}}}}}}}}}}}}SSSSSSS;S4446442SSSPSSPSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSUTSS");
unpack(8, -130.1, 3.94, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA8AAAHAAAAAALNQAAAAAZPWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAeeeeeejeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee?????????? ??????????????????????????F?????JVJ???6?OUH??????????AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAWAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAA;7AAA88888888888888888888888888888888888888888888888888888888T8888888AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAp~rVAAAA]bQMAAAAVOTJA");
unpack(9, -136.4, 2.867, "PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPWbgrPPP[dnc~PPPgkljjPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP[PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPFPPPPPPPPPPPPPPPPPPZ`bPPPPPaaaaPPPPamnrPPPPPahPPPPPPPPPPPPPPPPPPPHPPPPPPPPPPPPPPPPPPPPJPSPPPPP3PPPPPDE PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPDPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPEPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP@PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPXPPPPPPPXPPPPPPPPPPPDP PPPPPPPP");
unpack(10, -116.5, 2.759, "JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU\\UUUUUUUUUJJJJJJJJJJJJJJJJJJJJJJJJJJJJMJJJNQRSSRRPQJUVWVUSTUXYXWUUUVVWVVVVJJJJJJJJJJJJJJJJJJJJJJMJJJJJJOJNJJJROTJQJJQJJJUJJJJJJXJRJJJJWXJJUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUAAAAAAAAAAAA AAAAAA26AAAAAAAAAAGFAAAAAAHEAAAAAAEHAAAAAAEAAAAAAAAFFFF=FFFFF;788FFA974236?@?;;>6?=F??>@?=AFFA@?AAFFFFFCFFFFFFFFFIFCCCCCCCCCC=<=<CCC;:88C9=C=;789<>C>;::=>?@===>=>@@<?B@@=CCCCCCCCCJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJAAA5AAAAAAA64AAAAAA734AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
unpack(11, -129.5, 2.786, "NNNNNNNNNNNNNNNNNNNNNNNN^ZNNNNJNlb_NNNNN~xNNNNNNpt_NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNZNNNNNNNNNNNNNNSNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNGHNNNNNNDNNUNNNNNNYNNNNNNNNgNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNBNNNNNNN4NHNNNSN 5ANNNSNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKWGGGGGGGG<GGGGGGG7BGGGGGG6>GGGGGG7GGGGGGGGGGGGGGGAGGGGGGG<GGGGGGGCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN>BHNNNNN+:HNNWTNN=NNNUUNNNNNUVY");

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
