// loss: 0.07899    train id: 2024-01-04-11-27-42-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(218, 235), S(231, 239), S(225, 233), S(229, 239),
    S(238, 302), S(235, 307), S(239, 304), S(236, 306),
    S(294, 559), S(306, 577), S(301, 551), S(322, 566),
    S(610, 1101), S(595, 1137), S(613, 1101), S(606, 1147),
};
int DOUBLED_PAWN[] = {S(4, 16), S(-11, 14), S(8, 14), S(13, 10), S(14, 6), S(8, 16), S(-9, 13), S(2, 26)};
int PROTECTED_PAWN[] = {0, S(7, 7), S(6, 8)};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -21), S(6, -17), S(15, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_LOW "7MN732* @KG@=;+(?DSPMB0*DL[\\SG;2T\\|eb`E=@&b)%eBm1A84+,2631%'#$+7-5' ()=C,5859CLF#!7BEIWREUNAF]y@I32O> #-G64-Ys`f (02*2@>5FHO=;NM\"$ $(30)*84.()+8/28? +33)401$&+, (,/..33120+-*%% )8A75//#&5880-+(/..0240353/39:;AS?NQGOC}~:>?FKC0-$ %(3,2?3-'.92RVK@68AGpq`NFHUb?5([Tkz.?Au@@BL]\"2. 0GD2BUSE3:@- Lfk\\luye{(-p%27 -*)$)*+0034)252\"#' '&&%\"\"%$'*)'$ \"+*%*/->FB6GOV"
#define DATA_HIGH "                                           !!                                                                                                                                                                             !!!!!!                                !!!    !!!                                !! !!!                                                "

#define PAWN_OFFSET S(25, 74)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 8)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-43, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(4, -2)
