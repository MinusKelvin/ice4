// loss: 0.07825    train id: 2025-03-13-21-59-02-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(23, 81), S(127, 344), S(172, 386), S(272, 695), S(583, 1340), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(15, 0), S(24, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7RM852- :G=752%\"<CLIG;-&BMVWOA<.NXxhd]J;& E\\P>;3it¦  -4:=>6-NWZWV]N  ')%)%,&-!$ +9:: #*0622-AGC:570   !##%)'`W;' 89;GJ9 1-QP *-4'$'%$ # $01d'$ \"',70Z YY_bdW\\jkwÌ !(05'+)-$&//.()(*/&%\"'/.87+$\",3;LW:' .CEdf^206=D 8P\\aL>9#8LQNG:  \"//0.0='+)/,0+  !'265??133-%*% # AYeirg .8>=;53 '7DUb],&?JTKK: ;  \"* &$%\" 3W~»õ VG5,.--D @^kv} %4U¯Ț"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(24, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -26)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -61)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -36)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-60, 5)
#define BISHOP_LONG_DIAGONAL S(7, -1)
