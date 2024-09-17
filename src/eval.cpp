// loss: 0.07881    train id: 2024-09-15-18-05-40-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(231, 241), S(225, 236), S(229, 240),
    S(236, 306), S(234, 311), S(238, 308), S(236, 310),
    S(293, 566), S(305, 583), S(300, 557), S(322, 571),
    S(608, 1109), S(594, 1143), S(612, 1109), S(605, 1153),
};
int PAWN_SHIELD[] = {S(-4, 7), S(4, -17), S(7, -16), S(15, -11)};
int MOBILITY[] = {0, S(19, 33), S(5, -2), S(14, 20), S(9, 10), S(6, 14), S(-11, 19)};
int PASSER_RANK[] = {S(-8, -49), S(-13, -32), S(-8, -4), S(5, 29), S(7, 83), S(62, 129)};

#define DATA_STRING L"=UT=982&GTLECA3-EMXVTH80JU``YLD7X_njeQE- F[RA@=S>=YI&+7H% )OmR] (02*2@>6FHP=;NM\"$ $(30)*85.()+8/28? ,33)501#'+, ),/..33131,-+&%XXRSX^iaX XW\\_`V!(05)*'+$&..+&&'*/(&$(.,56,%%,26AK4$ +::OTN,,125#0, );8,>JIB97>3 Lfk]mvzf|q -))%)++0144)353\"#& '&'%\"\"%$')('% \"++&*0.>FC7GOV@i[MGHIA@ >R\\eku"

#define PAWN_OFFSET S(18, 74)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(42, 51)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-9, 10)
#define KING_RING_ATTACKS S(17, -6)
#define DIST_OFFSET S(-112, -64)
