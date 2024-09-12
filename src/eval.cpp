// loss: 0.07888    train id: 2024-09-12-16-50-18-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 236), S(229, 241), S(223, 235), S(224, 242),
    S(235, 306), S(232, 311), S(237, 307), S(234, 310),
    S(291, 564), S(304, 581), S(298, 556), S(322, 569),
    S(607, 1103), S(594, 1139), S(610, 1103), S(606, 1150),
};
int PAWN_SHIELD[] = {S(-4, 7), S(4, -17), S(6, -16), S(14, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(-6, -49), S(-13, -32), S(-8, -3), S(4, 30), S(7, 83), S(60, 129)};

#define DATA_STRING L"=VT<982%GTLECA3-FMXUSH80KV``YLD7X_\177mieRE/ FYQ@@>_IHeN&*=K' *VrVb )02+2@>6FHP=;NM\"$ $(30))85.')*7/28? ,32)5/1#&*, ),/..33231,,*%$Z[TTY`kcZ [Z_a`W!'05)*'+$&..+&&'*/(&$(.,56,%%,16AK4% +9:OTO--125#0, +>;-@LLD:8?4 Lgk]mvzf|p -)*%)++1144*353!#& '''%\"\"&%')('& \",*$)/.=FA7FNT?hZLEGG@? <P[dju"

#define PAWN_OFFSET S(17, 74)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-10, 10)
#define KING_RING_ATTACKS S(13, -5)
#define DIST_OFFSET S(-116, -62)
