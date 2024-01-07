// loss: 0.07849    train id: 2024-01-07-12-35-13-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(209, 306), S(220, 312), S(216, 305), S(216, 314),
    S(228, 383), S(226, 389), S(230, 385), S(226, 388),
    S(280, 701), S(290, 727), S(287, 693), S(307, 714),
    S(612, 1335), S(597, 1384), S(615, 1338), S(608, 1400),
};
int PAWN_SHIELD[] = {S(-1, -14), S(4, -25), S(5, -18), S(13, -11)};
int MOBILITY[] = {0, S(4, 9), S(1, 1), S(3, 7), S(2, 4), S(1, 7), S(-3, 6)};

#define DATA_STRING L"8RO621- APG?<:.(@ISOLB3*ER[ZRF?2Ubzc`^I<A+bfFm.A95,,14/1'($$)4*8) ((:?*6758@IB #8BDGRMG[SEJZt9YA@_G !6C64,KJ>S )/2*1@>6FHO=<NM#$ $(30))75.()*7/27? ,22)5/1$'+- )-//055343./-(' ,=I>:32%+<AA63/,65778;69=:8;@BDJbK[]S\\N²´°½Ê¼3.# $):/7E7-'0@7afUF9>LRrUMUduÇ·¡g_~®ÈÌPQV_{\"94 7XT9Wrr[N`gM Wsxgzq~¢§ 0/.&,-/57:9,7;:!$) *)*($$)%(++'+%#1* (/->F>3DLS"

#define PAWN_OFFSET S(24, 91)
#define PASSED_PAWN_OFFSET S(-15, -10)
#define BISHOP_PAIR S(23, 57)
#define DOUBLED_PAWN S(5, 20)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(23, 6)
#define ROOK_SEMIOPEN S(10, 14)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-8, 17)
#define KING_RING_ATTACKS S(13, -7)

int MATERIAL_FT[][4] = {
    {},
    {-9, 38, -136, 47},
    {-32, -33, 8, -17},
    {-21, -191, 18, -33},
    {2, -187, 1, -3},
    {-28, -209, 90, -64},
    {}
};
int MATERIAL_FT_BIAS[] = {-11, -38, 32, 35};
int MATERIAL_OUT[] = {48, 29, -35, 21, -23, 28, -32, 22};
#define MATERIAL_BIAS 6580
