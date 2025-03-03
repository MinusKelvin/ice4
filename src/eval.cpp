// loss: 0.07857    train id: 2025-03-03-12-47-23-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 68), S(143, 256), S(203, 289), S(300, 552), S(644, 1038), S(0, 0)};
int FORWARD_MOBILITY[] = {0, S(4, 8), S(1, 2), S(4, 6), S(3, 4), S(1, 5), S(-8, 6)};
int BACKWARD_MOBILITY[] = {0, S(0, 0), S(1, -4), S(1, 2), S(2, 1), S(1, 2), S(3, -5)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(15, 0), S(24, 0), S(15, 0), S(27, 0), 0};

#define DATA_STRING L"6QM862, ;H>971%\";CMIG<-'ANVXPB<.MXwfc_K;$ DUK<>4hr§  ,4;>>6-ALRQPZM  *($&'.&- $\".>@? #*0543,=A>725-  \"#%%&*(WE.! BRLIH8 1+PO ((,# %13h&% \"&,83Z [[`cdV<JLY²  %,1&)'*$%,,+'('(-%%\"&,*43*$#*05CL6(\"+:;TXS21458 :RZ^LGH$5GJHA6  +8:==DU&)*.+-*  \"(266AG.//*#%\"  +Kcov +5::71/ /:@JQG+#:CKDD6 6 !& /MmÍ K@1*++)< 8P\\ekp #1Kƣ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 48)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(22, -1)
#define ROOK_SEMIOPEN S(8, 9)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-9, -25)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 11)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-14, -51)
#define OWN_KING_PASSER_DIST_INDEX 138
#define OWN_KING_PASSER_DIST S(-6, 0)
#define OPP_KING_PASSER_DIST_INDEX 146
#define OPP_KING_PASSER_DIST S(-58, -28)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-28, 5)
