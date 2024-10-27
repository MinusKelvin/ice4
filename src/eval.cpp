// loss: 0.07862    train id: 2024-10-26-21-08-36-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(27, 70), S(139, 266), S(189, 303), S(301, 554), S(657, 1043), S(0, 0)};
int STORM_PAWN_RANK[] = {0, S(-6, -1), S(-8, 1), S(-2, -5), S(3, -14), S(25, -49), S(-27, -84)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(3, 5), S(2, 2), S(1, 4), S(-4, 1)};

#define DATA_STRING L"6PN761, :G>750%!:BKGE:-&>JRTM?:-?Mj\\XWD6/-CJ;2=7lx¤  -3:<=4,MX[WS\\M  ))$'%/&,!# *:9; \"*/50/*5;6.)2'  !\"$$%)&fa? !M]jFF6 1+MO +/8\" &24gOVLIMS`YO OPTWVJ@NP\\±  &-1%*(+#$++*''')-'%!&++88/'%,17S^L:3:GFV`bGDA<: 5JT\\I<9\"5GKJC7  \"./2./;%(',),'   %/209:,--)\"&# ! ?Vdena ,6==>9;#-=GQWL %6@G@@3 8#   .JiÖ,UJ<5555, +>IQW\\ $2NƊ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(23, 47)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(7, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(23, 3)
#define ROOK_SEMIOPEN S(8, 12)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -19)
#define KING_OPEN S(-41, -1)
#define KING_SEMIOPEN S(-10, 8)
#define KING_RING_ATTACKS S(12, -5)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-12, -46)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-94, -24)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-32, 4)
