// loss: 0.07905    train id: 2024-01-08-10-49-00-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 240), S(224, 233), S(224, 242),
    S(236, 302), S(233, 307), S(238, 303), S(234, 307),
    S(292, 555), S(305, 573), S(299, 548), S(322, 563),
    S(609, 1096), S(594, 1133), S(612, 1096), S(606, 1145),
};
int PAWN_SHIELD[] = {S(-3, -8), S(3, -21), S(5, -16), S(13, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QN521- AOF><:.(?HROLA3*FR[ZRF?2Ub{c`_I=?*`cFl2F:8//3444'*'',5,7(\"++=@,888<DND $7CFJVOBYN@F]x;V?>\\D #3I71+^xbk )02+2@>6FHO=;NM\"$ $'30))75.')*7/27? +32)4/1#&*+ (,/-.33131,,*%$ *9D:700#(6;;2.,)0/1236137436;;<AUARTIPD\177¡¡¨«¥2-% %(5.5?5-'/;5TVLA7:DJrrbMFJWd¢ZSl|¡ v@@BN_\"2/ 2JG3EXWH5<C/ Kfj\\luye{p -*)$(*+1134*353\"$' '''%\"#&$'**'% \",*#(/.=EA7FNT"

#define PAWN_OFFSET S(25, 72)
#define PASSED_PAWN_OFFSET S(-20, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 13)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 6)
#define ROOK_SEMIOPEN S(11, 11)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
#define ROOK_BEHIND_PASSER S(8, 16)
