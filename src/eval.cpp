// loss: 0.07874    train id: 2024-09-20-17-39-36-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(230, 241), S(225, 235), S(226, 243),
    S(236, 307), S(233, 312), S(238, 308), S(235, 312),
    S(292, 564), S(306, 582), S(299, 557), S(324, 571),
    S(609, 1107), S(596, 1142), S(612, 1107), S(608, 1155),
};
int PAWN_SHIELD[] = {S(-6, 8), S(4, -16), S(6, -16), S(13, -12)};
int MOBILITY[] = {0, S(4, 6), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-4, 5)};
int PASSER_RANK[] = {S(-9, -49), S(-12, -32), S(-8, -3), S(5, 29), S(5, 78), S(64, 129)};
int PHALANX_RANK[] = {0, S(0, 3), S(14, 8), S(16, 22), S(28, 49), S(119, 117), S(-18, 390)};

#define DATA_STRING L":UR9640\">K@:75)%?HPLI>1*FS[[SE@2T^~kfbPA' CTK;=7_HGeN%)<J' )TrU_ )02+2@>5FHO=;NM\"$ $(30))64-')*6/27? +32)4/0#&*, ),/./44120,,*%$Y[SSX_iaY [Y^__V\"+26*+*+!&.-+&'%&-%# %+(35*#\")04CM7'\",<<MQL+*./2#0, *=:-ALLD:8?4 Lgl^mvze{p -*)%)*+0134*353\"#' '&&%\"#&%')(&& \"-)$)/.<FB7FNT?fYLEFG?? ;OZcis"

#define PAWN_OFFSET S(23, 77)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(6, 5)
#define PROTECTED_PAWN S(12, 12)
#define ROOK_OPEN S(25, 3)
#define ROOK_SEMIOPEN S(10, 12)
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-9, 12)
#define KING_RING_ATTACKS S(13, -5)
#define DIST_OFFSET S(-114, -62)
#define DOUBLED_PAWN S(4, 14)
