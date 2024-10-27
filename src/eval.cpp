// loss: 0.0787    train id: 2024-10-28-10-38-51-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(25, 68), S(138, 265), S(189, 303), S(300, 553), S(657, 1043), S(0, 0)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(4, 5), S(2, 2), S(1, 4), S(-4, 0)};

#define DATA_STRING L"8RQ971, <H?861%\"<CKHF;-'BNVWPB</OYygc_L=& BUK;=5mx¤  -4:<=5,MXZWS[M  )(#'$.&-!# +::; \"*/5//)4:6-(1'   !##$)&\\Z= !O^nCK8 0-RL# &13i_aZZ^dqk_ `_dghZ<JLY·  %,2&+)+$%,.,((().'&#'-+55+$#*06EN7(\",<=WZU4267< 5JU\\J=9#6ILKD8  !./2/0<$(',)-'   &/219;,--(\"&\" \" ?Vdfna ,6=>>8; 6@HSYP\"&7AIBA3  /LlÊ=dZLEGGE= ;P[dio #1KƢ"

#define EG_OFFSET 156

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 14)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define KING_OPEN S(-46, 0)
#define KING_SEMIOPEN S(-12, 13)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 127
#define PASSER_RANK S(-13, -47)
#define KING_PASSER_DIST_INDEX 134
#define KING_PASSER_DIST S(-126, -58)
#define PHALANX_RANK_INDEX 149
#define PHALANX_RANK S(-28, 5)
