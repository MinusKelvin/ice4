// loss: 0.07847    train id: 2024-01-07-13-46-55-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(203, 223), S(215, 227), S(211, 222), S(211, 228),
    S(222, 277), S(220, 282), S(224, 279), S(220, 281),
    S(268, 509), S(278, 527), S(275, 503), S(295, 518),
    S(534, 1022), S(519, 1055), S(537, 1022), S(530, 1067),
};
int PAWN_SHIELD[] = {S(-1, -11), S(4, -18), S(6, -13), S(13, -7)};
int MOBILITY[] = {0, S(4, 6), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 4)};

#define DATA_STRING L"7RO621- APG><:.(?IRNLA3*ER[ZRF?2Tazc_^H<<)[~{_Be/A96--15/1()$%)5*8)!((:@*6758@IC #8BDGQMI[XPS`x@YAA_F !6@43+G@3M (/2*1?=5EGN<;ML#$ $(30))74-()*6/27? +22)4/1$'+- )-0.044242..,&% )5=52--#(487/-+)//0013/25313789>P?JLDJAyu-*\" #'3+1:0)%+70NRE;25?CjlZF@FQ\\|SMdtn65?HT\"2. 1HE2H[[KCPUB G\\`Tagk[mu{dt\177 ++*%)*+0133)143!#& '&'%##&$&'(%(#\",& %+)6<6-;AG"

#define PAWN_OFFSET S(23, 66)
#define PASSED_PAWN_OFFSET S(-16, -7)
#define BISHOP_PAIR S(22, 41)
#define DOUBLED_PAWN S(4, 14)
#define TEMPO S(10, 11)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(7, 6)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-44, -3)
#define KING_SEMIOPEN S(-8, 12)
#define KING_RING_ATTACKS S(14, -5)

#define MATNET_SIZE 8
#define MATNET_SIZE_X2 16
int MATERIAL_FT[][MATNET_SIZE] = {
    {},
    {-34, 53, -23, 33, -48, -1, 60, -26},
    {-19, 17, 4, -36, -67, 36, -1, -6},
    {-7, 9, 9, -180, -67, 24, -11, 0},
    {-11, -11, -13, -82, 45, -13, -2, 11},
    {-8, -18, -17, -221, -22, 45, -12, 87},
    {}
};
int MATERIAL_FT_BIAS[] = {-4, -108, -64, -43, 28, -29, 16, 90};
int MATERIAL_OUT[] = {-4, -27, -4, 31, 25, 9, 29, -20, 29, -17, -7, 31, 18, 3, 24, -18};
#define MATERIAL_BIAS 5260
