// loss: 0.07908    train id: 2024-07-06-12-36-33-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 234), S(228, 239), S(224, 233), S(223, 241),
    S(236, 301), S(233, 307), S(238, 303), S(234, 306),
    S(291, 558), S(304, 577), S(298, 551), S(321, 565),
    S(608, 1095), S(594, 1132), S(611, 1095), S(605, 1144),
};
int PAWN_SHIELD[] = {S(-3, -7), S(4, -21), S(5, -16), S(13, -9)};
int MOBILITY[] = {0, S(5, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7PN521, ANF><:-(?GROLB2*EPZYRF>2U`zb`^H<>(^bEk2D97--2631%)%%+7-5&!))<C+4659BLF! 5CEIUQFUNBH]x@V?>\\E #4H61*]vaj )02+2@>6FHO=;NM\"$ $'30))85-')*7/28? +32)5/1#&*, ),/..33131,,*$$ (7C85.0$&5;;1--*//2335244226:9<AR=QTFMD|¢¢¨©¥1.% &*6.4@6.(0<4TXMA7;DIrtcNGKXd¢\\To~ ¤w@@CO^\"3/ 3JG4EXXI6=C0 Lfk]luzf|p -**$(*+1144*353!#& '''&\"#&$'))'% \",*$)/.=FA7FOT"

#define PAWN_OFFSET S(23, 68)
#define PASSED_PAWN_OFFSET S(-16, -8)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(10, 11)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 13)
#define KING_RING_ATTACKS S(14, -6)
