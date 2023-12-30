// loss: 0.07876    train id: 2023-12-30-23-22-40-frzn-big3

#define S(a, b) (a + (b * 0x10000))

#define PHASE_BASE S(89, 124)
int PHASE[] = {0, S(6, 8), S(-6, -8), S(-7, -10), S(3, -13), S(7, -34), 0};

int QUADRANTS[] = {
    S(108, 81), S(120, 74), S(115, 75), S(126, 67),
    S(113, 106), S(113, 107), S(114, 106), S(116, 104),
    S(119, 237), S(125, 245), S(127, 227), S(142, 226),
    S(270, 469), S(260, 496), S(272, 467), S(274, 486),
};
int DOUBLED_PAWN[] = {S(1, 11), S(-9, 15), S(2, 9), S(7, 2), S(8, 0), S(2, 9), S(-8, 13), S(-1, 18)};
int PROTECTED_PAWN[] = {0, S(4, 1), S(3, 1)};
int PAWN_SHIELD[] = {S(3, -6), S(9, -16), S(10, -14), S(16, -17)};
int MOBILITY[] = {0, S(2, 3), S(1, -1), S(2, 1), S(1, 1), S(0, 2), S(-5, 7)};

#define DATA_LOW "7HE1..)!>F?865*';>HEA9+(>DMME:3+IObSMI831 I`\\O.I3=:<6589330/,-18-9,&-.;<)4627>B= #7;@AG@>PKFEJb9YDB]G\" :IB>@Q]R]# #%!#.-(238,+66$$ #(/,'(2/*'%(3,/39 +0/(3./%(+* &)+,,./1.-+,('%+!,?><=B'$28:6=>-/(+.5?@2/'&.8=D05 /66DBloaZ_c$s.'$!%)-(071/.05,GAB@5656\\UI?86<E!y]B;GO_d`I%%. @$== =ffFHY[O=9D1 >BD?GDHAFIJCLMO\"'+)! \"''\"%(%+)\")(' 5-*)-%*(0/,,.+(,$%&' -1/&26<"
#define DATA_HIGH "                                                                                                                                                                                                                              !                                 !                                                                                               "

#define PAWN_OFFSET S(2, 17)
#define PASSED_PAWN_OFFSET S(-19, -11)
#define BISHOP_PAIR S(11, 19)
#define TEMPO S(7, 2)
#define ISOLATED_PAWN S(5, 1)
#define ROOK_OPEN S(18, -9)
#define ROOK_SEMIOPEN S(6, 3)
#define KING_OPEN S(-30, 18)
#define KING_SEMIOPEN S(-9, 14)
