// loss: 0.07883    train id: 2024-09-16-11-40-16-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(231, 241), S(224, 236), S(229, 240),
    S(236, 306), S(234, 311), S(238, 308), S(235, 310),
    S(293, 565), S(305, 582), S(300, 557), S(322, 571),
    S(608, 1109), S(594, 1143), S(611, 1108), S(605, 1153),
};
int PAWN_SHIELD[] = {S(-4, 7), S(4, -17), S(7, -16), S(15, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(-8, -49), S(-13, -33), S(-8, -4), S(5, 29), S(7, 83), S(61, 129)};

#define DATA_STRING L">VU=:93&GUMFDB4.FNYVTI91KVaaYME8Y`nkfSF. GZSAA>\\FEbM*.;I( &VsXb (02*2@>6FHP=;NM\"$ $(30)*85.()+8/28? ,32)501#'+, ),/..33131,-+&%VWPPV\\g_V ZUZ\\]T!(05)*'+$&/.,&&'+0(&$(.,66,%%,26AK3% +:;OTN-,125#0, *<9-@KKE:7>4 Lgk]mvzf|q -))%)++0144)353\"#& '&'%\"\"%%')('% \"++&*0.>FC7GOUAi\\NHIJBA >S^glw"

#define PAWN_OFFSET S(17, 74)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-9, 10)
#define KING_RING_ATTACKS S(4, -2)
#define DIST_OFFSET S(-108, -66)
