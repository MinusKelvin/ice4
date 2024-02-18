// loss: 0.07906    train id: 2024-02-19-10-08-11-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 239), S(224, 232), S(224, 241),
    S(233, 302), S(231, 307), S(236, 303), S(232, 306),
    S(292, 557), S(305, 575), S(299, 549), S(322, 564),
    S(605, 1100), S(591, 1136), S(608, 1099), S(603, 1149),
};
int PAWN_SHIELD[] = {S(-3, -7), S(3, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-4, 5)};

#define DATA_STRING L"7QO632- APG?=;/(?ISOLB3*FR[ZRF@2Ub{c`_I=@+adGm0A85++1410$'##)5,4& ((;A+5759BKE!!6BDIUQCVMAG\\w>V?>\\D #3H71+^wai )02+2@?6FHO=<NM#% %)41)*85.(*+8/27? +32)4/1#&*, '*---0012.)-)\"\" *9C9600#(6;:1.,)0/1135036426:;<AU@QTHOC~  §ª£1-$ %(4-4?4-'.:4SVK@69CHqqaMGIVc [Tl{ v@@BM^\"2. 2JG3EWWH5<B/ Lfk]luye{p -+*$(*,1144*353!#' '''%\"#&$'))'# #-($*1,:FB3FOT"

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
#define QUEEN_BISHOP_BATTERY S(7, -9)
