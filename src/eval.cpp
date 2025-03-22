// loss: 0.07825    train id: 2025-03-22-12-45-02-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(22, 82), S(127, 344), S(175, 384), S(270, 697), S(578, 1343), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(15, 0), S(24, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7QN962- :G<861&\"<CMIG;-'AMVWPA;.NWxgc^I;&\"F[P><3it\177¥  ,39<=5,LVXVRZL  )(#'$.&-!# *8:: #*0622,AGC:570  !\"$$&*'`W;& 89=GJ9 1-QP *-4'$'%$ # %01c'$ \"',71Y YX^acV]jlxÌ  '/4'+),$%..-'(').&%!'.-87*$\"+3;LV9' .CDce^106<C 8P\\bM>:#9MQOH:  \"002/1=',+1-1+  !'254??123-%)$ # BZfjsg ,7=<:42 '7DUb],&?JTKK: ;  \"* '$%\" 4X\177¼ö UG5,.--D @^kv} %4U¯Ț"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -26)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -62)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-57, -36)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-61, 5)
#define QUEEN_NEAR_KING S(3, 6)
