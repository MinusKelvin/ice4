// loss: 0.07905    train id: 2024-02-19-10-18-15-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 239), S(224, 232), S(224, 241),
    S(237, 301), S(234, 307), S(239, 302), S(234, 306),
    S(291, 558), S(304, 577), S(298, 550), S(321, 565),
    S(610, 1097), S(595, 1135), S(614, 1097), S(607, 1147),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7QN521- AOF><;.(?IROLB3*FR[ZRF@2Ub{c`_I=A+adGn0B85++0410$'##)5,4& ((:A+5759BKE!!6BEIUQCWNBG]w>V??]D #4H71+^waj (/2*2@>6FHO=;NL\"$ $'30))75-')*7/27= +21)4/0#&)+ '+.--22210*.($\" *9C9600#(6;:1.,)0/1235036426:;<AU@QTHOC~  §ª£1-% %(5.4?4-'.:4SVLA79CIqqaNGIVc¡[Tl{ v@ABM_\"3/ 2JG3EXWH5<C0 Lfk]luze{p -**$)*+1144*353!\"& &&&%!\"%$&)(&$ #-($)0+<FB1FOU"

#define PAWN_OFFSET S(25, 73)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(25, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
#define ROOK_OPP_QUEEN_FILE S(8, 9)
