// loss: 0.07902    train id: 2024-01-17-13-29-30-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 239), S(224, 233), S(224, 241),
    S(237, 301), S(234, 307), S(238, 303), S(235, 306),
    S(293, 557), S(305, 576), S(300, 549), S(321, 565),
    S(609, 1095), S(594, 1133), S(611, 1096), S(606, 1146),
};
int PAWN_SHIELD[] = {S(-3, -7), S(3, -21), S(5, -15), S(13, -9)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QO622- APG?=;/(?ISOLB3*FR[ZRF@2Ua{b`_I=A+aeGm0A85++042/$'##)5,4& ((;B+4769BKE !6CEIUQCVNBF\\w>R<;XB #1H62-[t`h )02+2@>6FHO=;NM\"$ $'30))75.')*7/28? +32)4/1#&*, (,/-.33220,+)$$ *9C9700#(6;:1.,)0/1135036426:;<AU@QTHPD~  §ª£1-% &)5.4?4-'/:4SVKA79CIqqaNGJVc¡[Tl{ v@ACM_\"2. 1IF3DWVG4;B/ Lfk\\luze{p -*)$)*+0034*353\"#' '''%\"\"%$&)(&% \"+)\"(.-;D@5DMS"

#define PAWN_OFFSET S(25, 73)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(11, -7)
#define KING_RING_DOUBLE_ATTACKS S(31, 1)
