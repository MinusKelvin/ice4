// loss: 0.07863    train id: 2025-02-27-22-48-16-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 70), S(143, 265), S(189, 301), S(301, 552), S(644, 1054), S(0, 0)};
int MOBILITY[] = {0, S(4, 7), S(1, -1), S(4, 4), S(2, 2), S(1, 3), S(-3, 1)};
int KING_ATTACK_WEIGHT[] = {0, S(36, 0), S(20, 0), S(27, 0), S(19, 0), S(26, 0), 0};

#define DATA_STRING L"7QN972, :G=861%\"<CMJG;-'BNVXPB<.OXygc_K=%!DWM=>5ht\177¨  -4:=>6-NX[XU_O  ))$'%.'-!# ,<?< #*0622+<B=5/3,  !\"#$%)'QG/! BRZDH9 0,OM +.5\" $03h&& \"&+72\\ ]\\aefY<JLYµ  &-2%)'*#$,,*&&%'+$# %+)32(\"!).4BK5'!+:;SVQ0/238 4ISWE:6$6HKIC7  \".-/,.9&)).+.)   %.0-58...*#&# ! ?U`ahX ,6;:821(/;BMTJ %8AIBB5 ;\" \" 0MmÎ J@1*,+*< 8P\\ejo #0Jơ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-16, -22)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 12)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-13, -50)
#define OWN_KING_PASSER_DIST_INDEX 138
#define OWN_KING_PASSER_DIST S(-6, 0)
#define OPP_KING_PASSER_DIST_INDEX 146
#define OPP_KING_PASSER_DIST S(-60, -28)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-28, 5)
#define QUEENLESS_ATTACK S(-20, 0)
