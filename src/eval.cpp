// loss: 0.07884    train id: 2024-09-12-15-02-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(212, 228), S(226, 232), S(220, 227), S(222, 233),
    S(230, 273), S(229, 278), S(232, 274), S(230, 277),
    S(287, 532), S(301, 549), S(295, 524), S(319, 536),
    S(611, 989), S(598, 1024), S(614, 991), S(610, 1036),
};
int PAWN_SHIELD[] = {S(-2, 5), S(5, -19), S(7, -17), S(15, -11)};
int MOBILITY[] = {0, S(6, 12), S(5, 3), S(10, 27), S(7, 19), S(5, 41), S(-7, 17)};
int PASSER_RANK[] = {S(-7, -48), S(-12, -31), S(-7, -2), S(5, 31), S(6, 82), S(61, 127)};

#define DATA_STRING L"@YW@>:5(GTLECA3-EMXUSH8/KVaaYME8Y`njeSF/ FZR@@?aJIgP&+?M' ,YsVd */2*2?>3DFM;9KJ#% $'20)(64.%)*7/38@ -43)501#'+- ),/-/34020,+*%%XZQQW^icX ZX\\^^S!'/4(*'+!#+,)$$$',%#!&+)33*#\"*/4AJ4% +9:PUO..346#1- +><-ANMD:9@4 Pej\\jrvdxn +*'#&'(-/11'/11\"#' '&&%\"\"%$&('&' #/+#',/;B=7BIM?fYLEFF>? ;OZciu"

#define PAWN_OFFSET S(16, 73)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(25, 1)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-10, 9)
#define KING_RING_ATTACKS S(14, -5)
#define DIST_OFFSET S(-112, -62)
