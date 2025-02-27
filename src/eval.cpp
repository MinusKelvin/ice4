// loss: 0.07863    train id: 2025-02-26-21-41-48-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 70), S(143, 265), S(189, 302), S(301, 553), S(642, 1056), S(0, 0)};
int MOBILITY[] = {0, S(4, 7), S(1, -1), S(4, 4), S(2, 2), S(1, 3), S(-3, 1)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(16, 0), S(24, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7QN962, :G<861%\"<CMIG;-'BMVXPB</OXxgc_K=% CVL<>5ht\177§  -4:=>6-MX[XT]N  ))$'%.', \" +;=; #+0622,<B=5/2,  !\"#$%)'QI0! CSZDI9 1,PM +-4# %14i&& \"&+82[ \\[adeY=KMZ´  &-2&)(*#%,,+&&%'+$# %+)32(\"!).4BL5'!+:;SVQ0/238 5JTXF:7#5HKHB6  \".-0-.9%)(-*-(   %.0-58-..)\"&\" ! @VabiW +5;:720*0<CNTK %8AIBB5 ;\" \" 0MmÎ J@1*,+*; 8P[djn #0Jơ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
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
#define OWN_KING_PASSER_DIST_INDEX 138
#define OWN_KING_PASSER_DIST S(-6, 0)
#define OPP_KING_PASSER_DIST_INDEX 146
#define OPP_KING_PASSER_DIST S(-59, -27)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-29, 5)
