// loss: 0.07905    train id: 2023-12-31-17-43-49-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(229, 239), S(224, 233), S(224, 241),
    S(236, 301), S(233, 307), S(238, 303), S(234, 306),
    S(292, 557), S(305, 576), S(299, 550), S(322, 564),
    S(609, 1095), S(595, 1132), S(612, 1095), S(606, 1144),
};
int DOUBLED_PAWN[] = {S(4, 16), S(-11, 14), S(8, 14), S(13, 10), S(14, 6), S(8, 16), S(-9, 13), S(2, 26)};
int PROTECTED_PAWN[] = {0, S(7, 7), S(6, 8)};
int PAWN_SHIELD[] = {S(-3, -7), S(3, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_LOW "7MO632* ALG?=;+(?ESOMB0*EM\\\\TG;2T\\|eb`E=@#b)$e@l2C95+-2631%'#$*7-5' ((<C,5759BLF\"!7BEIVREWN@G]z@V?>\\D #4H71+_wbj )/2+2@>6FHO=;NM\"$ $'30))75-()*7/28? +32)4/1$%*, (,/-.33121,,*$$ )8A75//#&5980-+(/./0240353039:;AS@NQGNC} :??GLC0-$ %(4,3?4-'.:2RVKA79BGpraNGIVb@6)\\Tl{/@@uAABL^\"2/ 2JG3EWWH5<B/ Lfk\\luye{(.p&28 -*)$)*+1134*353!#' '''%\"#&$'*)'% \",)$(/-<EA6FNT"
#define DATA_HIGH "                                           !!                                                                                                                                                                            !!!!!!!                                !!!    !!!                                !! !!!                                                "

#define PAWN_OFFSET S(25, 74)
#define PASSED_PAWN_OFFSET S(-16, -10)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 8)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
