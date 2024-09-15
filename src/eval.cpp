// loss: 0.07886    train id: 2024-09-15-12-37-08-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 236), S(230, 240), S(224, 235), S(227, 241),
    S(236, 306), S(234, 310), S(238, 307), S(235, 309),
    S(291, 564), S(304, 582), S(298, 557), S(322, 570),
    S(607, 1107), S(593, 1141), S(610, 1107), S(604, 1151),
};
int PAWN_SHIELD[] = {S(-4, 7), S(4, -17), S(7, -16), S(15, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(-7, -49), S(-13, -32), S(-8, -4), S(5, 29), S(7, 83), S(61, 129)};

#define DATA_STRING L"=VU<982&GTLECA3.FMYVTH80KVaaYLE7X_njeRE. GZR@@>_IHeO(,=K( )TsWa )02+2@>6FHP=;OM\"$ $(30))85-()*8/28? ,32)5/1#'*, ),/./33131,-+&%XYRRW^iaX YX\\__V!(05)*'+$&..+&&'*/(&$(.,56,%%,16AK4$ +9:OTN-,125#0, +=:-@LLD;8?4 Lgk]mvzf|q -)*%)++1144*353!#& '&'&\"\"&%')('& \"++&*0.>FB7GOV@i[MGHIA@ >Q\\ekv"

#define PAWN_OFFSET S(17, 74)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(10, 10)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-9, 10)
#define KING_RING_ATTACKS S(6, -2)
#define DIST_OFFSET S(-112, -64)
