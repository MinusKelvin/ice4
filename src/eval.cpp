// loss: 0.07916    train id: 2023-12-30-19-11-20-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 233), S(232, 237), S(224, 232), S(238, 235),
    S(236, 300), S(236, 304), S(237, 301), S(240, 302),
    S(289, 555), S(304, 573), S(297, 547), S(325, 560),
    S(607, 1087), S(597, 1123), S(611, 1088), S(617, 1133),
};
int DOUBLED_PAWN[] = {S(4, 17), S(-11, 14), S(7, 14), S(13, 10), S(13, 7), S(8, 16), S(-9, 13), S(4, 26)};
int PROTECTED_PAWN[] = {0, S(7, 7), S(6, 8)};
int PAWN_SHIELD[] = {S(5, -10), S(12, -24), S(14, -19), S(21, -13)};
int MOBILITY[] = {0, S(5, 7), S(1, 0), S(4, 5), S(2, 3), S(2, 4), S(-4, 5)};

#define DATA_LOW ";PP843* EOJB?<+(CGVQNB/*IP^]TF;1Xa~ic`D;A&b)\"d?k2D;7-.4852&'$%+9.7)!**>D,797:DNG# :?EJXSEWNAH\\z?cMKiJ !7N=5.h!iq )/2*1@>6GIQ=<ON\"% $(30),95.)(,:/28? -42)611\"$*) )+..-21431+,)%$ )8B8601#&5990.,)/./025145304:;=AS?MQHPE~~:?@HMD0-$ %(4,3?4.(/:3RVK@7:CHpq`MGIVbA7)]Ul{/ABvAACL_\"2/ 4NK6GZZK6>D1 Lfk]muye{(-q&27 -**$)*+0034*452!#' '&&%\"\"%$(*)'& \"+)$)-,<E?6FLR"
#define DATA_HIGH "                                           !!                                                                !                                                                                                            !!!!!!                                !!!    !!!                                !! !!!                                                "

#define PAWN_OFFSET S(24, 74)
#define PASSED_PAWN_OFFSET S(-18, -10)
#define BISHOP_PAIR S(23, 47)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 8)
#define ROOK_OPEN S(25, 2)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-10, 16)
