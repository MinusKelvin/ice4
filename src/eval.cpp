// loss: 0.07823    train id: 2025-03-22-16-03-24-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(22, 81), S(127, 345), S(176, 386), S(271, 696), S(538, 1409), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(39, 0), S(19, 0), S(27, 0), S(24, 0), S(26, 0), 0};

#define DATA_STRING L"7RN973- :H=862&\"<DMJG<.'BNVXPB=/NVwgc^J;&\"E[Q?=3iu¤  ,39<=5,JUWTPXJ  )(#'$.&.\"$ *764 #*0511,AGB:580  !\"#$%)'aZ>& :;@GJ9 1-QP ),3'$'%$ # %02d'$ \"',70Z YY_bdWUcdpÁ  (05'+)-%&//.(((+/'&\"(/.98+$\",3;MX;( .CEdg_206<C 7O\\bN?:#8MQOH:  \"11522?&*)/+0*  !'378CD123-&*% \" C\\hlui .9@?>64 %4AR`[(&?KTKK; :  #* '$%! 3W~»õ VG5,.-.E B_lw} %4V²ȝ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 17)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-13, -26)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-9, 11)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -61)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -37)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-53, 5)
#define DEFENDER_NO_QUEEN S(-89, 0)
