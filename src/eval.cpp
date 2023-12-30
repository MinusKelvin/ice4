// loss: 0.07899    train id: 2023-12-30-22-09-35-frzn-big3

#define S(a, b) (a + (b * 0x10000))

#define PHASE_BASE 510
int PHASE[] = {0, 16, -12, -16, -24, -66, 0};

int QUADRANTS[] = {
    S(104, -81), S(123, -50), S(118, -55), S(138, -22),
    S(115, -65), S(117, -61), S(117, -62), S(124, -48),
    S(133, -198), S(137, -210), S(143, -176), S(160, -161),
    S(306, -329), S(299, -356), S(311, -321), S(317, -332),
};
int DOUBLED_PAWN[] = {S(0, -16), S(-17, -37), S(0, -11), S(11, 9), S(12, 12), S(2, -10), S(-14, -30), S(-2, -25)};
int PROTECTED_PAWN[] = {0, S(5, 4), S(4, 3)};
int PAWN_SHIELD[] = {S(2, 7), S(10, 24), S(13, 24), S(23, 36)};
int MOBILITY[] = {0, S(2, -3), S(4, 8), S(4, 4), S(2, 0), S(0, -2), S(-9, -20)};

#define DATA_LOW "Ed]:54- Q^OF@@-)KN`\\VF/*MYhk\\I</\\^}nc]E>E;h+'lQ[2@AFA<=@0174.02=(;- +.@@&3407@G=!1IKNFF>DZ\\d_RV9xRM V1/I: '?:20<, &)$$53-8:>41;<*( $/:6,.>9/-'-;28>H 388.?6:-475 '/3359;;9:98463i9\"EA@7 {0jYPR5/ln) uZ4-hz37}[F-vmH,umFB%+W yU-%0NUZQF?H+':8.1*@e#xj\"%85J_s|)45z F'CI*uZZnB'|O@~/A;9EVV1` .leLHn\\&(+- 4/23/08''(9/ &=JD24MA36'/FBKQe AGH;WFN8CIF )89@DGIKA@EJ=?4"
#define DATA_HIGH "                                           !!                                                      !                                                                             !!      !        !!      !!      !!       !    !!!!!!!!!!!!!!!! !  !!!!    !!!   !!!!    !\"!!! \"!!\"!  !                                                                        "

#define PAWN_OFFSET S(-9, -81)
#define PASSED_PAWN_OFFSET S(-19, -117)
#define BISHOP_PAIR S(25, 12)
#define TEMPO S(10, 9)
#define ISOLATED_PAWN S(7, 5)
#define ROOK_OPEN S(25, 36)
#define ROOK_SEMIOPEN S(9, 6)
#define KING_OPEN S(-44, -71)
#define KING_SEMIOPEN S(-10, -24)
