// loss: 0.07882    train id: 2024-09-17-14-37-53-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(230, 241), S(224, 236), S(228, 241),
    S(236, 306), S(234, 311), S(238, 308), S(235, 310),
    S(292, 565), S(305, 583), S(299, 557), S(322, 571),
    S(608, 1108), S(594, 1143), S(611, 1108), S(605, 1152),
};
int PAWN_SHIELD[] = {S(-5, 7), S(4, -17), S(6, -16), S(14, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(-8, -49), S(-13, -32), S(-8, -4), S(5, 29), S(7, 83), S(62, 129)};

#define DATA_STRING L"=UT<982&GTLECA3-EMXVTH80JU``YLD7X_njeRE. G[RA@>T?=ZI&+7H% )PmR] )02*2@>6FHP=;NM\"$ $(30)*85.()*8/28? ,32)501#'+, ),/./33131,-+&%XXRRX^iaX XW\\_`V!(05)*'+$&..+&&'*/(&$(.,56,%%,16AK4$ +::OTN-,125#0, );8,?JJB97>3 Lgk]mvzf|q -))%)++0144)353\"#& '&'%\"\"%%')('% \"++&*0.>FB7GOU@i[MFHIA@ >Q\\eku"

#define PAWN_OFFSET S(18, 74)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-9, 9)
#define KING_RING_ATTACKS S(8, -3)
#define DIST_OFFSET S(-112, -64)
