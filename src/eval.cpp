// loss: 0.07846    train id: 2024-01-07-15-47-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(205, 220), S(217, 224), S(212, 219), S(212, 226),
    S(225, 273), S(222, 277), S(227, 275), S(223, 277),
    S(275, 499), S(285, 517), S(281, 493), S(301, 508),
    S(543, 1005), S(529, 1036), S(546, 1005), S(540, 1047),
};
int PAWN_SHIELD[] = {S(-1, -10), S(3, -17), S(5, -12), S(13, -7)};
int MOBILITY[] = {0, S(4, 6), S(1, 0), S(3, 5), S(2, 2), S(1, 4), S(-3, 4)};

#define DATA_STRING L"7RO621- APF><:.(?IRNKA3*ER[ZRF?2Ubzc`^H<>+]\177{`Cf.@85,,14/0'(#$)5*7( ((:@*5748@IC!#8BDGRMJ[VOS`wAZBA`G !7=44)A?9N )/2*1@>6FHO=<NM\"$ $(30))74-()*7/27? +22)4/1$'+- (,0./44232--+'& )4<52--#(477/-+)//0013/25313789=N>IKDJ@xs-)\" #&2*09/)%*5/LPD:14>BhiXD?DOZyQLaql32<FP\"1. 1GD2H[ZJCOSA GZ^R_eiZksybr| ++*$(*+/022)133!#& '&'%##&$%''%&!!*& %*(5;6-:@E"

#define PAWN_OFFSET S(23, 64)
#define PASSED_PAWN_OFFSET S(-15, -6)
#define BISHOP_PAIR S(23, 39)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(10, 10)
#define ISOLATED_PAWN S(8, 6)
#define PROTECTED_PAWN S(7, 6)
#define ROOK_OPEN S(23, 4)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-43, -3)
#define KING_SEMIOPEN S(-8, 12)
#define KING_RING_ATTACKS S(13, -4)

#define MATNET_SIZE 4
int MATERIAL_FT[][MATNET_SIZE] = {
    {}, {}, {}, {}, {}, {}, {}, {},
    {},
    {-4, -30, 44, -65},
    {63, 16, -24, 101},
    {87, 43, -43, 69},
    {-142, -105, -15, 29},
    {-37, -2, -54, -13},
    {}, {},
    {},
    {-92, -77, 48, -43},
    {-37, -1, -10, -132},
    {-56, -16, -29, -44},
    {147, 94, -11, 27},
    {-33, 6, -65, -206},
    {}, {},
    {}
};
int MATERIAL_FT_BIAS[] = {48, 134, 69, -35};
int MATERIAL_OUT[] = {36, -42, 21, -23};
#define MATERIAL_BIAS 5919
