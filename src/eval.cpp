// loss: 0.07911    train id: 2024-01-06-17-34-55-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 233), S(229, 239), S(224, 232), S(224, 241),
    S(236, 301), S(234, 306), S(238, 302), S(234, 306),
    S(292, 556), S(304, 574), S(299, 548), S(321, 563),
    S(608, 1092), S(594, 1129), S(611, 1092), S(606, 1142),
};
int PASSED_PAWN_RANK[] = {S(3, 0), S(-5, 7), S(-3, 27), S(9, 48), S(14, 86), S(44, 53)};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -21), S(5, -16), S(13, -11)};
int MOBILITY[] = {0, S(5, 7), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"5PM400-!?NE=:9.)=GQMJ@3,BPYYQF@3LWwgedPA' Snme^QW@?]C #3E4/)Yu^e )02+2@>6FHO=;NM\"$ $'30))75-')*7/28? +32)4/0#&*+ ),/..33131,,*%$ )17,.-0\"'/0-),,)0)%$)2179/%%.7=N[>*%3HI¶¶¨zy\"3/ 3KG4HXWK8<C2 Lfk\\luze{p ,**$(*+0134*352!#' '''&\"#&%'*)'% \",)#(/.<EA6FNS"

#define PAWN_OFFSET S(26, 79)
#define BISHOP_PAIR S(23, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 8)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-9, 15)
#define KING_RING_ATTACKS S(14, -6)
