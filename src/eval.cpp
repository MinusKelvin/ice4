// loss: 0.07863    train id: 2025-02-27-22-59-49-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 70), S(143, 265), S(190, 302), S(301, 553), S(640, 1058), S(0, 0)};
int MOBILITY[] = {0, S(4, 7), S(1, -1), S(4, 4), S(2, 2), S(1, 3), S(-3, 1)};
int KING_ATTACK_WEIGHT[] = {0, S(34, 0), S(18, 0), S(27, 0), S(17, 0), S(30, 0), 0};

#define DATA_STRING L"7QN972, :G=861%\"<CMIG;-'BNVXPA<.OXxgc_K=% CVL<>5hs\177§  -4;>>6-MXZWT]N  ))$'%.&, # +;>< #*0622,<B=503,  !\"#$&*(PG/! BRYDH9 1,PM +.4# %14i&% \"&+71\\ ]\\befY=KMZµ  &-2&)(*#%,,+&&%'+$# %+)32(\"!).4BK5'!+:;SUQ0/237 5JTXE:7#5HKIC6  !--/,.9&)(-*.)   %.0-58-..)\"&\" ! @V`aiW +5:972/)/;BMTJ %8AIBB4 ;! ! 0NnÎ J@1*,+*; 7P[djn #0Jơ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-15, -22)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 12)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-14, -51)
#define OWN_KING_PASSER_DIST_INDEX 138
#define OWN_KING_PASSER_DIST S(-6, 0)
#define OPP_KING_PASSER_DIST_INDEX 146
#define OPP_KING_PASSER_DIST S(-60, -27)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-29, 5)
#define KING_ATTACK_BIAS S(-10, 0)
