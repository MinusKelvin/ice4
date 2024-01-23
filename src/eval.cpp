// loss: 0.07991    train id: 2024-01-23-22-43-27-frzn-big3-qsfiltered

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(231, 240), S(241, 244), S(238, 238), S(237, 245),
    S(247, 296), S(244, 302), S(249, 297), S(247, 300),
    S(316, 539), S(329, 556), S(321, 533), S(344, 545),
    S(590, 1093), S(576, 1128), S(593, 1094), S(590, 1134),
};
int PAWN_SHIELD[] = {S(-1, -4), S(4, -19), S(5, -15), S(13, -10)};
int MOBILITY[] = {0, S(4, 7), S(1, 0), S(3, 6), S(2, 4), S(1, 4), S(-3, 5)};

#define DATA_STRING L"6PO742- ?OG?=:.'>ISPNB4*DQ\\\\RG?2Uay\\a^I;N@ryR}0?85+-36/,!%##+7.3& &(<D2976;FNJ$*>LFU]VHTORYLGP;:VB #2G$%/^cav (.1(1>=4CGL;<ML\"% %'32*'74/'*+:/17> ,32(3/0\"&*, (+.+-33/1.+*(%$ )6?65//#'5991.,)/.11340362159:;;M>MIBG>cpqtis~i3/& \"'006B5.&-95RVK@78CJoqaNGFUaVVcvWUTUx\"0. /GC1?WUB(78# I_bUcgn]nw~fx +*'$#$'/*--(--)$%' &'%#!!%%'('&# &/,&&-0=D<7CHM"

#define PAWN_OFFSET S(28, 76)
#define PASSED_PAWN_OFFSET S(-17, -9)
#define BISHOP_PAIR S(26, 44)
#define DOUBLED_PAWN S(5, 15)
#define TEMPO S(20, 24)
#define ISOLATED_PAWN S(9, 9)
#define PROTECTED_PAWN S(7, 4)
#define ROOK_OPEN S(22, 3)
#define ROOK_SEMIOPEN S(10, 6)
#define KING_OPEN S(-41, -3)
#define KING_SEMIOPEN S(-9, 15)
#define KING_RING_ATTACKS S(13, -6)
