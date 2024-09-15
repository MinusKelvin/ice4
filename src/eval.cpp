// loss: 0.07875    train id: 2024-09-15-17-49-42-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(230, 241), S(224, 235), S(226, 242),
    S(235, 307), S(233, 312), S(237, 308), S(234, 311),
    S(292, 566), S(305, 583), S(299, 558), S(323, 572),
    S(609, 1107), S(596, 1143), S(612, 1108), S(608, 1155),
};
int PAWN_SHIELD[] = {S(-6, 8), S(4, -16), S(6, -16), S(13, -12)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(-7, -48), S(-11, -31), S(-7, -2), S(4, 30), S(5, 81), S(62, 127)};
int PHALANX_RANK[] = {0, S(0, 4), S(14, 8), S(16, 22), S(28, 48), S(120, 117), S(-18, 388)};

#define DATA_STRING L"<US;861#@KA;97*'AIQNK@2,GS[\\TFA3V^~lhcPB+ EXO=?;_IHeN&*<J' )TqU` )02+2@>6FHO=;NM\"$ $(30))74-')*7/28? ,32)4/1#'*, ),/./44131,,*%$Z[TUZ`kcZ \\Z_a`W!(04()'*\"%--+%&&',$#!%+)43)#\")/5BJ4% *:<QTO.-126#0, +=;-ALLE;9@5 Lgl^mvzf|q -))%)*+0144*353!#& '&&%\"\"&%')('& \"-*$)/.=FB8GOU=eXJDEF>= :NYbhr"

#define PAWN_OFFSET S(20, 72)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(7, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 10)
#define KING_OPEN S(-45, -2)
#define KING_SEMIOPEN S(-10, 10)
#define KING_RING_ATTACKS S(13, -5)
#define DIST_OFFSET S(-116, -58)
