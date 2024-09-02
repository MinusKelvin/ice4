// loss: 0.07913    train id: 2024-09-02-22-29-30-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 233), S(228, 238), S(224, 232), S(223, 241),
    S(236, 301), S(233, 306), S(238, 302), S(234, 305),
    S(291, 557), S(304, 575), S(298, 549), S(321, 564),
    S(607, 1093), S(594, 1129), S(611, 1092), S(605, 1142),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -22), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(5, 2), S(-3, 11), S(-2, 30), S(10, 51), S(14, 91), S(44, 58)};

#define DATA_STRING L":SQ9540&DQIA?=1-BKUROE70GS]]VJC8QYzkjhTF+ Trqh`VW@?]D #4E4/)Yt]f )02+2@>6FHO=;NM\"$ $'30))75-')*7/28? +32)4/1#&*, ),/..33231,,*%$ '06+-,0#&.0.)+-*/(&%*2287.%%.7>MX<($2GI¶¶§yx\"3/ 4KG4HYXK9=D3 Lfk\\lvzf|p ,**$(*+1144*353!#' '''&\"#&%'*)'% \",)#)/.=FA7FNS"

#define PAWN_OFFSET S(20, 74)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 10)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 13)
#define KING_RING_ATTACKS S(14, -6)
