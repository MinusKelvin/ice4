// loss: 0.07863    train id: 2025-02-27-08-41-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 70), S(143, 266), S(189, 302), S(301, 552), S(642, 1057), S(0, 0)};
int MOBILITY[] = {0, S(4, 7), S(1, -1), S(4, 4), S(2, 2), S(1, 3), S(-3, 1)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(16, 0), S(24, 0), S(16, 0), S(27, 0), S(-13, 0)};

#define DATA_STRING L"7QN962, :G<861%\"<CMIG;-'BMVXPB</OXxgc_K=% CVL<>5ht\177§  -4:=>6-MX[XT]N  ))$'%.', \" +;=; #+0622,<B=5/2,  !\"#$%)'RJ1# CT\\DI9 1,PM +-4# %14i[[UW[`lf[ \\[adeY=KMZ´  &-2&)(*#%,,+&&%'+$# %+)32(\"!).4BL5'!+:;RUP0/237 4ISWE:6#5GJHB6  \".-/,.9&)(-*.( ! &/1.68...*#&# ! @U`ahW +5;:720)0<CNUK &8AIBB5 ;\" \" 0MmÎ;e[LEGFE; 8P\\eko #0Jơ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(23, 47)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-15, -23)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 12)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-14, -51)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-118, -54)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-29, 5)
