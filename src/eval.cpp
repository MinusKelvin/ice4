// loss: 0.07906    train id: 2024-01-16-10-30-51-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 239), S(224, 233), S(224, 241),
    S(236, 301), S(233, 307), S(238, 303), S(234, 306),
    S(292, 557), S(304, 576), S(299, 549), S(322, 564),
    S(608, 1094), S(594, 1132), S(611, 1094), S(606, 1144),
};
int PAWN_SHIELD[] = {S(-3, -8), S(3, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QN621- APG><;.(?IROLB3*FR[ZRF@2Ub{c`_I=A+adGm0B85++141/$'##)5,4& ((:A+5759BKE!!6BEIUQCVNBG]w>V?>\\D #3H71+^wai )02+2@>6FHO=;NM\"$ $'30))75-')*7/28? +32)4/1#&*, ),/..33131,,*$$ *9C9700#(6;;1.,)0/1235036426:;<AU@QTHOC~  §ª£1-% %)4.4?4-'/:4SVLA79CIqraNGIVc¡[Tl{ vAACM_\"2/ 2JG3EWWH5<B/ Lfk\\luye{p -*)$(*+1134*353!#' '''%\"#&$'))'% \",*#(/.<EA7FNT"

#define PAWN_OFFSET S(25, 73)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
