// loss: 0.07898    train id: 2024-01-04-12-00-24-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(218, 235), S(231, 240), S(225, 233), S(230, 240),
    S(238, 302), S(235, 307), S(239, 304), S(237, 307),
    S(294, 558), S(306, 578), S(301, 551), S(322, 567),
    S(610, 1102), S(594, 1138), S(613, 1102), S(605, 1149),
};
int DOUBLED_PAWN[] = {S(4, 16), S(-11, 14), S(8, 14), S(13, 10), S(14, 6), S(8, 16), S(-9, 13), S(2, 26)};
int PROTECTED_PAWN[] = {0, S(7, 7), S(6, 8)};
int PAWN_SHIELD[] = {S(-3, -7), S(4, -21), S(7, -16), S(15, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_LOW "7MN732* @LG@=;+(?DSPMB/*DM[\\SG;2T\\|fb`D=A'b)%eCm1@74+,2630%'#$+7-5' ()=C,5859CLF#!7ADIWREUNAF]y@H31O> #-G87/Xsbf (02*2@>5FHO=;NM\"$ $(30)*84.()+8/28? ,32)501$&+, (,/..33120+-*&% )8A75//#&5880-+(/./0240353/39:;AT?NQGOC}~:??FLC0-$ %(3,3?3-'.92RVK@69AGpraNGIUb?6)[Tlz/?@t@@BK]\"1. /FC1ATRD29?- Lfk\\luye{(.p&27 -*)$)*+0044)353!#' '''&\"#%$&))&$ !**%*/->FB5FOV"
#define DATA_HIGH "                                           !!                                                                                                                                                                             !!!!!!                                !!!    !!!                                !! !!!                                                "

#define PAWN_OFFSET S(26, 74)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 8)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-43, -4)
#define KING_SEMIOPEN S(-9, 15)
#define KING_RING_ATTACKS S(-1, -4)
#define KING_RING_ATTACKS_SQ S(5, -1)
