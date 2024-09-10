// loss: 0.07909    train id: 2024-09-10-19-38-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 235), S(228, 240), S(224, 233), S(223, 242),
    S(235, 302), S(233, 307), S(238, 303), S(233, 306),
    S(291, 559), S(304, 577), S(298, 551), S(320, 566),
    S(607, 1096), S(593, 1132), S(611, 1096), S(605, 1145),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -22), S(5, -16), S(13, -9)};
int MOBILITY[] = {0, S(5, 10), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(5, 6), S(-3, 15), S(-3, 34), S(8, 57), S(11, 106), S(58, 139)};

#define DATA_STRING L"\\ut[WVSHfskca_SOdmwtqgYRiu\177\177wleZt{vh< ^\177ugfW@?]D #4F50*Yu_h )/2+2@>6FHO=;NM\"$ $'30))85-')*7/28? +32)5/1#&*, ),/..33231,,*%$*09?365:.09:74675:4105==BA8..7@GOX;% 0FLtue76@OX#3/ 4KG4HYXK8=D3 Kfk\\lvzf|p -**$(++1244+363!#' '''&\"#&%'*)'% \",*$)/.=FB7FNT"

#define PAWN_OFFSET S(-14, 61)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 10)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 14)
#define KING_RING_ATTACKS S(14, -6)
