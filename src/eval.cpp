// loss: 0.07822    train id: 2025-03-22-17-47-04-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(22, 81), S(126, 344), S(176, 385), S(271, 694), S(473, 1374), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(19, 0), S(28, 0), S(25, 0), S(25, 0), 0};

#define DATA_STRING L"7RN973- :H=862&\"<DMJG<.'BNVXPB=/MVwgc^I;&\"E[Q?=4ju¤  -4:==5-JUXTPWJ  )(#'$.&-\"$ *764 #*0611,=C>626.  !\"$$&*(aZ>% ;=BGJ9 1-QP ),3'$'%$ \" $/1c'$ \"',70Z YY_bdWP^_k¼  (05'+)-%&//-(((*/'&\"(/-97+$\",3;MX;( .CEdg_206<C 8P]cO@;#8LQOH:  \"12522?&*)/,0*  \"(389DD123-&+& \" BZfgo` .8?><42 &4AS`[(%?JTKK: :! #* '$%! 3W\177¼õ VG5,--.E B^lw} %4V²ș"

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
#define PASSER_RANK S(-13, -61)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -37)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-48, 5)
#define ATTACKER_NO_QUEEN S(-179, 0)
#define DEFENDER_NO_QUEEN S(77, 0)
