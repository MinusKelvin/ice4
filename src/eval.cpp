// loss: 0.05212    train id: 2025-10-03-18-02-52-sirius-mixed

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(15, 105), S(118, 423), S(179, 468), S(275, 824), S(729, 1462), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(43, 0), S(27, 0), S(30, 0), S(24, 0), S(29, 0), 0};

#define DATA_STRING L"7[X<=;6'=OA@=<-,AHUTTE5/KX`b[PG;ZhxmhTJ9 RdgICRn}»  0;BFH>3M^`][cA  )'#%$0%5 +(3?B\\ \",8<@/)8?72,.$  &*'(--)vqE& 8ÎrjS D:qs 19D$\"%#  %$ -8g($ %&.<)g ljlnq[ .1=\177ü )8=.0++##821,-&.6.+$)4/?B7+*):@RWJ.\"9IMhxoC1-HY =`pxgR9)F]ecWF  ';AF?CF&35:8:0  %+=FM`K245,(('  %Wv *3FHB=; -HVi{z8\"ETfR[E <#  -!'&*  0_ÎĨ _Q6,,-8I Hfu} 0:_Ìǁ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(21, 78)
#define TEMPO S(11, 8)
#define ISOLATED_PAWN S(6, 12)
#define PROTECTED_PAWN S(12, 18)
#define ROOK_OPEN S(36, 2)
#define ROOK_SEMIOPEN S(14, 10)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-19, -28)
#define KING_OPEN S(-50, -1)
#define KING_SEMIOPEN S(-13, 10)
#define MOBILITY_INDEX 131
#define MOBILITY S(0, -3)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -65)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-8, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-71, -41)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(0, -2)
#define ATTACKER_NO_QUEEN S(-75, 0)
