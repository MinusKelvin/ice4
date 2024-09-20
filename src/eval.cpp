// loss: 0.07872    train id: 2024-09-23-23-34-43-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(230, 241), S(225, 235), S(226, 243),
    S(236, 307), S(233, 312), S(238, 308), S(235, 312),
    S(292, 565), S(306, 583), S(299, 557), S(324, 571),
    S(609, 1107), S(596, 1143), S(612, 1107), S(608, 1155),
};
int PAWN_SHIELD[] = {S(-6, 8), S(4, -16), S(6, -16), S(13, -12)};
int MOBILITY[] = {0, S(4, 6), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-4, 5)};
int PASSER_RANK[] = {S(-9, -50), S(-13, -33), S(-8, -3), S(4, 29), S(5, 79), S(63, 129)};
int PHALANX_RANK[] = {0, S(0, 4), S(14, 8), S(16, 22), S(28, 49), S(119, 117), S(-21, 389)};

#define DATA_STRING L";VR:751#?LA;86*&@IQMJ?2+GT\\\\TFA3U_\177kgcQB( DUL;=8^HGdN%);K' )VrV` )02+2@>5FHO=;NM\"$ $(30))74-')*6/27? +32)4/0#&*, ),/./44120,,*%$YYSTX_kaY [Y^``V!*15)*)+!%.-+%&$&,%# %+(34*#\")/4CM7'\",;<NSN-+/03#0, *=:-@KKD:7?4 Lgl^mvze{p -*)%)*+0134*353\"#' '&&%\"#&%')(&& \"-)$)/.<FB7FNT?i[MFGG@? ;Q]flv"

#define PAWN_OFFSET S(22, 75)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(6, 6)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(25, 3)
#define ROOK_SEMIOPEN S(10, 12)
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-9, 12)
#define KING_RING_ATTACKS S(13, -5)
#define DIST_OFFSET S(-114, -62)
#define DOUBLED_PAWN S(5, 9)
