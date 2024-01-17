// loss: 0.07907    train id: 2024-01-17-13-12-25-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 238), S(224, 232), S(225, 240),
    S(236, 301), S(234, 306), S(238, 302), S(235, 305),
    S(292, 557), S(305, 575), S(299, 549), S(322, 563),
    S(608, 1093), S(595, 1130), S(611, 1093), S(607, 1141),
};
int PAWN_SHIELD[] = {S(-2, -7), S(4, -21), S(6, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QO621- APG><;.(@ISOLB3*FS\\ZRF@2Uc|c`_I=E.deIo0B85++041/$'##(5+4& (':A+5759BKE!!7BEIUQCVMBG\\v=V@?]D \"3F60(\\wag )02+2@>6FHO=;NM\"$ $(30))75.')*7/27? +32)4/1#&*, ),/..33131,,*%$ *9C9700#(6;:1.,(0/1235036326:;<AT@QTHPD}\177  §ª£1-% %)5.4?4-'/;4SVKA7:CIqqaMGIVc¡[Tl{ v@ABM^\"2/ 3KH4FYXJ6=C1 Lfk]luye{p -**$)*+0034*352\"#' ''&%\"#&$'*)'% \",*$)0.=FA7FNT"

#define PAWN_OFFSET S(25, 73)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(13, -5)
