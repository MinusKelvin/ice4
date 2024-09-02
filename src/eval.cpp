// loss: 0.07912    train id: 2024-09-03-21-59-32-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 234), S(228, 239), S(224, 232), S(223, 241),
    S(236, 301), S(233, 306), S(238, 302), S(234, 305),
    S(291, 558), S(304, 576), S(298, 550), S(321, 564),
    S(607, 1094), S(594, 1130), S(611, 1093), S(605, 1142),
};
int PAWN_SHIELD[] = { S(-3, -7), S(4, -21), S(5, -16), S(13, -10) };
int MOBILITY[] = { 0, S(5, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5) };
int PASSER_RANK[][6] = { { S(8, 1), S(-2, 12), S(-9, 37), S(1, 60), S(-4, 101), S(33, 89) }, { S(2, 3), S(-3, 8), S(3, 22), S(16, 42), S(27, 81), S(53, 29) } };

#define DATA_STRING L"5NL30/+ ?LD<98,(=EPMJ?1+BNXXOD=2OZzp\\\\I<1&ZxbZRGW@?\\E #4H4/+\\u^h )02+2@>6FHO=;NM\"$ $'30))85-')*7/28? +32)4/1#&*, ),/..33131,,*%$ '06,-,0#&.0/),-)/(%'+3366,#(18@HR7!+8LNZ­·\"3/ 2JG4FXWI7<C1 Lfk\\lvzf|p -**$(*+1144*353!#' '''&\"#&%'*)'% \",)#)/.=FA7FOT"

#define PAWN_OFFSET S(25, 74)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(10, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 7)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 14)
#define KING_RING_ATTACKS S(14, -6)
