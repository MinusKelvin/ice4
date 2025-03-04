// loss: 0.07859    train id: 2025-03-04-14-51-40-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(24, 70), S(147, 265), S(191, 302), S(301, 557), S(642, 1070), S(0, 0)};
int MOBILITY[] = {0, S(4, 7), S(0, -1), S(4, 4), S(2, 2), S(1, 4), S(-5, 2)};
int KING_DEFEND[] = {0, S(-5, 7), S(5, -1), S(2, 0), S(3, -4), S(1, -13), 0};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(16, 0), S(25, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7RN:83- :G=962&\"<DMJG<.(BNWXPB=0OYyhd_L=&\"EXN=>6fr~ª  ,4:==3*KWYWS\\M  ))$'%-&, #!-=@> \"*/520)9?:3.2,  !\"#$%)'GB, !I\\XDK: 0-RM *-4\" %14i%$ !%*60\\ \\\\aeg[=KLZµ  $+/$*(+#$+,+''&'+$#!%,*33(\"!)/5CL5'!+:;TVQ0/238 4IRVC96#6GJHB6  \"/.0-/:%)(-*-(   %--*23,--)\"$\" \" =OVTZF +6=>=63+09@JQG &7AJCB3 :\" \" /Mn Ï J@1*,,+= 9R^glp #0JƟ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 14)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -20)
#define KING_OPEN S(-43, -2)
#define KING_SEMIOPEN S(-10, 11)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-14, -51)
#define OWN_KING_PASSER_DIST_INDEX 138
#define OWN_KING_PASSER_DIST S(-5, 0)
#define OPP_KING_PASSER_DIST_INDEX 146
#define OPP_KING_PASSER_DIST S(-60, -29)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-29, 6)
