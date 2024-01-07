// loss: 0.07851    train id: 2024-01-07-13-13-23-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(203, 218), S(214, 222), S(210, 217), S(210, 223),
    S(222, 271), S(219, 275), S(224, 272), S(220, 274),
    S(268, 495), S(278, 512), S(275, 489), S(295, 504),
    S(532, 998), S(518, 1028), S(535, 998), S(529, 1039),
};
int PAWN_SHIELD[] = {S(-1, -10), S(4, -17), S(5, -12), S(13, -7)};
int MOBILITY[] = {0, S(4, 6), S(1, 0), S(3, 4), S(2, 2), S(1, 4), S(-3, 4)};

#define DATA_STRING L"8RO621- APG><:.(@IROLA3*ER[ZRF?1Ubzc`^H<=*\\~z^Be.@86,,14.0')$$(5*7) ((:?*5648?HB $8BDFQLK]YQS`w@ZBB`G !7?11,BB9P (/2*1?=5EHN<;ML#$ $(30))74-()*6/27? +22)4/1$'+- (,0.044242..,'& )4<52--#(377/-*)//0013/24213679=N>IJDI@wr-)\" #&2*09/(%*5/LPD904>BgiXD?DOZxPKapj20;FO\"1- 1GD1H[ZJCPSA GZ^R_eiYkrxbq| +**$)*+/022)133 \"& '&'%\"#&#%''$'#\"*& %*(4;5-9?E"

#define PAWN_OFFSET S(22, 63)
#define PASSED_PAWN_OFFSET S(-15, -6)
#define BISHOP_PAIR S(23, 38)
#define DOUBLED_PAWN S(4, 14)
#define TEMPO S(10, 10)
#define ISOLATED_PAWN S(8, 6)
#define PROTECTED_PAWN S(7, 6)
#define ROOK_OPEN S(23, 4)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-8, 11)
#define KING_RING_ATTACKS S(13, -4)

#define MATNET_SIZE 4
#define MATNET_SIZE_X2 8
int MATERIAL_FT[][MATNET_SIZE] = {
    {},
    {-79, -36, -136, -19},
    {-64, 12, -21, -7},
    {-74, 23, -40, 18},
    {-74, 6, -56, -25},
    {66, 43, 64, -17},
    {}
};
int MATERIAL_FT_BIAS[] = {-106, 166, -8, -66};
int MATERIAL_OUT[] = {94, -35, -44, 12, 105, -35, -42, -14};
#define MATERIAL_BIAS 14997
