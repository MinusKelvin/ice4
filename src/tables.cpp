#define NEURONS 12
int16_t FT[25][SQUARE_SPAN][NEURONS];
int16_t BIAS[NEURONS] = {-27, 46, 230, -73, -77, 82, 133, 118, 139, -22, 2, 171};
int OUT_W[2*NEURONS] = {172, 57, -38, -26, 60, 70, 99, 92, -114, -57, 61, -99, -168, -58, 39, 27, -60, -71, -95, -107, 113, 56, -61, 98};
#define OUT_B 4303

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
unpack(0, -45.83, 1.27, "DDDDDDDDDDCDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDbbbbbbbbbbgbbbbbbbbbbbbbbbbbbbbbbb{~bbbbmbbbbbbbbbbbbbbbbbbbbbbbvpvmlvlvvvvvtvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv]```````U``````U``````````````````````````````````c`````````````aaa^aaaaaaaaaaaaaaaaaaaaaaaaaaghaaaaarlaaaauxaaaaaaaaaaaaaaxaaaaDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD0DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD(*)DDD,'\"% %575DD.(43D644234DD<:76::DDDD?>=DDDDDDDDDD3333333333333333333333333333)33333333333333333333337333333333<33.--,,,,-DD+)*,/0D41/1.04DD65557:DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD6DDDDDDDDDDDDDDDDDDDDDDDDDDDDD:DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");
unpack(1, -42.87, 4.494, ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,./,,,,,5Xk,%,,,wi~,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,/,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,-----------------------------1---------------------3------------000000000000000000000000040000000000000000000000000000000000000077777777777777777;777777;@=>:777ACAA7777CFIB7777EE7A777777777777****************************1************************* *********'''''''''''''''''''''''''''''''''''''(',''''''.0'# ''-1-''''''''*******************************************************1*/***-00*************************%***********************&***************************************************************/*****&/,*************************************************************************************************************.******04****-0FH****69JD");
unpack(2, -177.9, 2.836, "____________________________________l_________________ _____________________________________________V__________________________________j___________________________________________________________________________________________________________________________________________________________________________________________________k____________________________________________________bbbbbbbbbbbbbbbbbbbv[bbbbbRuGvbbbbQ{@zWbbbN|>|UbbbV~CxZbbbbbbbbb____________________________________________________________________________________________________________g________________________________________________________f_____________________g_e__________________________________________________________________________________________________________________________________");
unpack(3, -321.9, 5.015, "````````````````````````````````````````````````2```````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````h```````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````````X```````````````````````e`h`X````~`T```````````````````````````````````````````````````````````d```````````````````````````````````````````````````````````````````````` ````````````````````````````````````````````````````````````````R`ej```hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh````````````````````````````````````````````````````````````````");
unpack(4, -161.1, 4.468, "DDDDDDDDDDDDDDDBDDDDDDD@DDDDDDD?@DDDDDD?DDDDDSMDDWbDULDQDDDDDDDDMJMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMSMMMMMMMMMMMMMMMMMIIIIIIIIIIIIIIIIIIIIIIKIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMffffffffffffffffffffffjfffffffffffjffffffnfff~zfffsfyffsffffffffDDDDB?DDDDD45?DDD:,! +DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD3D;DDDDDDDDD?DDDDDDDDDDDDDDDDDDDDDDADDDDDD?@DDDDDDDDD@@@@@@@@@@8@@@@5@@@@6@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@B@DDDDDDDDDDDDDDDDDDD:D=D@DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDBDDDDDDDDDDDDDDDD:DDDDDDDDDDDD;DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDEDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD8DDDD<DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD7DSDDD8");
unpack(5, -127.6, 2.232, "YYYYYYYYYYYYYYelYYYYYY_lYYYYYSYfYYYYYYYYYdyfYYYYYYY~rYYYYYYYYYYYYYYYYYYJYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYbYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYdYYYYYYYYYYYYYYYYYYYYYY\\YYYY YYYYYYY.YYYYYYYYYYbbaYYYYiYYYYYYYYYYYYYYYtYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY`YYYYYYYYnYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYNYpYYYYYYYgnYYYYYYYYYYYfYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSWSSSSSSSS]SSSSSSS^SSSSSSSZSSSSSSSSYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYNYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYaYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY");
unpack(6, -426.3, 8.179, "VVVVVVVVVVVVZVXVVVVVVVVVVVVVVVVVVVVVVVVXVVVVVYVVVVVVVVYVVVVVVVVV\\V\\\\\\\\\\\\\\\\\\\\\\\\\\\\Z\\\\]\\]\\\\\\\\]^^^\\\\\\\\\\\\^\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\[[X[[[[Y[Y[[[[[[[[[][[[[[][[[[[[[[[[[[[[[[[[[[[[[[[^[[[[[[[[[[[[dddddddddddddddddddddddddddddeddddddddddddddddddddddddddddddddddyyytyystyyyyyyyyyyyyyyyyyyyy|{yyyy|y~yyyyyyyyyyyyyyyyyyyyyyyyyyyQTRPSTTTTTTTTTWTRTSTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTQQQQQQQQ'' $(#QJQNQQQQQQQQQRQQQQQQQSRQQQQQQQQQQQQQQPQNQQQQQQQQQQIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIOIIIIIIIIIIIIIIIIIIGIFIIIIIIIIIIIIIIHIIIIIIIIIIIIIIIIIIIIIIIIIIINIIIIIIMAAAAAAAAAAAAAAAAAAAAAAAA@AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA,,,,,,,,,*,,,,,,,,,,,,*,,,,,,+*,,,,,,,*,,,,,,,,,,,,,,,,,,,,,,,,,TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTUTTTTT");
unpack(7, -139.0, 4.161, "AAAAAAAAcXIAA>>?TLFAA>>?HAAAAAAAAAAAAAAAAAAAAHFAAAAAAAHAAAAAAAAAAAAAAAAAAAAAAAAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALFFFFFFFLFFFFFFFFFFFFFFFSPFFFFFFRLFFFFFFQLFFFFFFNFFFFFFFFFFFFFFFGGGNGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGZGGGGGGGGGGGGGGGGGGGGGGGGGGGGGAUAAAAAAA~UAAAAAA_RIAAAAAALGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA'&3AAAAA %4AAAAA.28AAFEA8:AAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAA;AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeemeeeeeeeeeeeeeeeeeeieeeee");
unpack(8, -69.17, 1.259, "FFFFFFFFFFFFFFFFFFFFFFFFFFFF5FFFFF;?-?8FFFF. FFFFFFFFFFFFFFFFFFF^WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWPQSWRWWWWNHJSWWWWKACFOWWMIWCDEWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWGWWWWWWWWWWWWWWWWWWWWWWWWWRWWWWWWWWWPWWWWWWWWWWWWWGWWWWWWWWWW6WWWWWWW4WWWWWWWWWWWWfWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnannnnnnn^nncn^nn]WkcnnnnnnnnnnnnWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWhWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWJWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWclWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWmqtxWWWWWWsWWWWWWWWWWWWWWWWWWWWWWWWW`WWWWWWW\\WWWWWWWWWWWWWWWWWWWWWWWWWWW~WWWWWfWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW");
unpack(9, -334.5, 5.037, "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD                                                                bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
unpack(10, -621.0, 11.21, "ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~WWWWYWWWWWWWWWWWWWWWWWWWWWZWW[WWWWWWWW\\WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW)(WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWYWWWWWWWWWWWWWRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM                                                                WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWZWWWWWW]\\WWWWWWWWWWWWWW");
unpack(11, -358.4, 8.014, "MMMMMMMMMMMMMKKMMMMMJKJMMMLMMMKMMMMMMMMMMMMMMMMOMMMMMMSMMMMMMMMM@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@B@@@@@@@@@L@@@@@@HBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB6666666666666666666666666664666646666666266666661666666666666666(((((((((((((((((((((((((((((((((((((((((((((((((((((((((( (((((MPNMMMMMMMMMMMMMPMMMMMMMMMMMMMMMMMMMMMMMMMOSMMMMimdMMMMc`MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMNMMMMMMMMMMMMMMMMMMMMMMMMMMMJMKMMMMMMMMYY]]^YYYY\\^YYY]YYYYYYYYYYYYYYYYY\\YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYZZZZZZZZZZZZZ\\ZZZZZ[Z\\ZZZZ[ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffzww}ww~}wwwwww}wxwwwwwwwxwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwMUWUUUWMWW\\Z[^fXQVXWWZZSMRRSSSROMMMMMMNMMMMMMMMMMMMMMMMMKJMMMMMM");

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
