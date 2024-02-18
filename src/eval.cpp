// loss: 0.07904    train id: 2024-02-18-21-06-25-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 234), S(229, 241), S(223, 233), S(228, 243),
    S(237, 301), S(234, 306), S(238, 303), S(235, 305),
    S(291, 558), S(304, 577), S(298, 551), S(321, 566),
    S(611, 1093), S(600, 1122), S(615, 1091), S(614, 1122),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -21), S(6, -16), S(13, -10)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 3), S(-3, 5)};
int KING_RING_ATTACKS[] = {0, S(32, -17), S(10, -9), S(12, -5), S(15, -6), S(10, 11), S(0, 0)};

#define DATA_STRING L"7QN622- APF><;.(?IROLB3*ER[ZRF@2S]w]]]H<A*`cGm2C97--2630%)%%+7-4&!))<C,586:CLF %8GHKWRDXODH_x?S<<YE #4I83,Zxad )02+2@>6FHP=<NM\"$ $(30))75-')*8/28? +32)4/1#&*, (,/-.32120++)$# *9C:700#(7;;2/,)102346137537;;<BWCUWIPD~¢¡¨«¤1-$ %(4-4?5-'.:3TWLA69BIssbOGJVc¢ZSl{¡wAACM_\"2/ 0HD0BVTE4;A. Lek\\ltye{p -*)$(*+1134*352!#' '''%\"#&$'))&% #/,',5/@IF;LRY"

#define PAWN_OFFSET S(25, 72)
#define PASSED_PAWN_OFFSET S(-18, -10)
#define BISHOP_PAIR S(23, 47)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-8, 15)
