// loss: 0.07822    train id: 2025-03-22-16-07-04-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(23, 81), S(128, 343), S(177, 383), S(276, 691), S(580, 1334), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(19, 0), S(28, 0), S(25, 0), S(26, 0), 0};

#define DATA_STRING L"7QM862- :G<851&\";CLIG;-&AMVWPA<.MUwfb]I:&!E[P>=3jv¥  ,39<=5,KVXUQYK  ))$'%.&-!# )654 \"*/500+AGB:591  !\"$$&*'aZ>& :;@GJ9 1-QO *-3'$'%$ \" $/1c'% \"',70Z XY^bdWWefrÄ  (05'+),$&//-(((*/'%\"'/-87+$\"+3;LX:( .CEdf^206<C 7O\\bN?;#8LQOH:  \"11522?&*)/,0*  \"(388DD122-%*% # BZfirg -7>=;52 '6CTb]*%>JTJJ: :  #* '$%! 3W~»ô UG5+---E B^kv| %4V±ȗ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -25)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-9, 11)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-13, -60)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -37)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-55, 5)
#define ATTACKER_NO_QUEEN S(-97, 0)
