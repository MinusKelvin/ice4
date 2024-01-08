// loss: 0.07904    train id: 2024-01-08-12-11-29-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 233), S(228, 240), S(224, 231), S(223, 242),
    S(236, 301), S(233, 308), S(238, 302), S(234, 308),
    S(292, 559), S(304, 578), S(298, 551), S(321, 566),
    S(608, 1095), S(594, 1134), S(611, 1095), S(606, 1146),
};
int PAWN_SHIELD[] = {S(-3, -8), S(3, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 4), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QO622- APG?=;/(@ISOMB4+FS\\[SG@3Ub{c`_I=@,adGl1C85,,1520$'$$)6,4& ((;B,5759BLF\"!7CEJVQEWODJ^w?V?>\\D \"4I72,aycn )02+2@>6FHP=;NM\"$ $(30))85.')*7/28? +32)4/1\"&*+ (,/..33131,,*$$ *8B8500 $277.+(&-+-./2-141/2789AT?OQGOC||¤§¡.+$ $(2*2>5.&.92RUKA79AGppaNGIUb[Tky¡£xDBFQb\"3/ 2JF2EXXI5<B/ Mgl\\lvzdzp -+*$)*,0034*353!#& &&&%\"\"%$'))'% #-)$)/-;EA7FOU"

#define PAWN_OFFSET S(24, 78)
#define PASSED_PAWN_OFFSET S(-18, -5)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 15)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(10, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-8, 14)
#define KING_RING_ATTACKS S(14, -6)
#define BLOCKED_PASSER S(5, -23)
