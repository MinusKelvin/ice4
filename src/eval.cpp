// loss: 0.07821    train id: 2025-11-08-18-48-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(23, 80), S(128, 342), S(177, 383), S(276, 691), S(580, 1333), S(0, 0)};
int PAWN_ISLANDS[] = {S(27, -18), S(11, -4), S(3, 13), S(-7, 20), S(-13, 31)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(19, 0), S(28, 0), S(25, 0), S(26, 0), 0};

#define DATA_STRING L"7PK750,!;G;650&#<BJHF:-'AMTWOA</MUveb]I<& D[P><4jv¦  ,39<=5,KVYURYK  )($'%.&-!# )654 \"*/500+AGB:591  !\"#$%)'`Y=% :;@GJ9 1-QO *-3'$'%$ # $.1c%$ \"&+6/X XX^acVVceqÄ  )38*-*,$'11/)('*0)'#)/-88,&$-3:LX<*\"/CDbf_206;A 7P\\cO@;#8LQOH:  !01422>&*)/,0*  !(378DD012,%*% \" AYfirg -8?><53 '5BS`[(%>JSJJ: ;! #* '$%! 3W\177¼ö SE3*+++H C_mx~ &5V±ȗ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(4, 13)
#define PROTECTED_PAWN S(11, 17)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -25)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-9, 11)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-12, -62)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-5, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-56, -40)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-54, 5)
#define ATTACKER_NO_QUEEN S(-97, 0)
