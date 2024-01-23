// loss: 0.07834    train id: 2024-01-24-10-48-53-frzn-big3-d1-nocap

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(231, 237), S(242, 240), S(238, 236), S(238, 240),
    S(247, 295), S(244, 300), S(249, 295), S(247, 297),
    S(315, 535), S(329, 549), S(321, 529), S(345, 539),
    S(599, 1077), S(585, 1113), S(602, 1078), S(599, 1118),
};
int PAWN_SHIELD[] = {S(-1, -3), S(4, -19), S(5, -15), S(13, -10)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 6), S(2, 4), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QP852- @OH@>;/'>JTQNB4*DR]\\SH@2Tcy`e_J<M+n~\177\177K0@94*-360.\"$##+7.3' %(;D0775;ENI$'8FAO[U=aJBL?'P::VD #3T63>nln (/1)1?>4DGM;<ML\"% $'32*(75/'*+:/07> ,43)301\"%*, (*-+,33/0.+)($# )6?750/$'5991.-)/.1235136215:;<=RBOLGKA«®/+# !&/,2>2,%,71PTI>56AGno`LEFS_£\\Zh| z{U_TJh#2/!/GD1<TR>)88  G]`Sbfm\\mw}dw *'$#!\"$,(+*%+*'#%& %&$\"  ##&(&%! &/,'(.0>E>8CJO"

#define PAWN_OFFSET S(28, 74)
#define PASSED_PAWN_OFFSET S(-17, -7)
#define BISHOP_PAIR S(26, 43)
#define DOUBLED_PAWN S(5, 15)
#define TEMPO S(20, 20)
#define ISOLATED_PAWN S(9, 9)
#define PROTECTED_PAWN S(7, 5)
#define ROOK_OPEN S(22, 3)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-41, -3)
#define KING_SEMIOPEN S(-10, 16)
#define KING_RING_ATTACKS S(13, -6)
