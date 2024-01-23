// loss: 0.07849    train id: 2024-01-24-10-05-57-frzn-big3-qsfiltered-2

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(231, 238), S(242, 240), S(238, 236), S(238, 241),
    S(248, 295), S(245, 300), S(250, 296), S(247, 298),
    S(316, 535), S(331, 549), S(322, 528), S(346, 538),
    S(596, 1081), S(582, 1116), S(599, 1082), S(596, 1121),
};
int PAWN_SHIELD[] = {S(-2, -3), S(4, -19), S(5, -15), S(13, -10)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 6), S(2, 4), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7PO742- @OG?=:/'>ISQNB4*DR]\\SG@2U`x]a^I;L,lz}{E0@95+-260.\"%##+7.4& &(;D1876;ENI!(7IDR\\U?^GCNC'O99VD #2T74<lnm (/1)1?>5CGM;<ML\"% $'32*'74/'*+:/16> ,43(3/0\"%*, (+.+-33/0.+)($$ )6?7500$'5:92.-)//1245146216:;<=TBPNHLB¨¡±0,$ \"'/-3?3-%,72QUJ?57BHnpaMFFUa¤[Xh| ~YaTIl#2/ /GD0;SR>)88  G^aSbfl[mv|dv )'$\"!!#,(**%*)&$%' %%#\"  ##&(&$\" '/-''.0>E>8DJO"

#define PAWN_OFFSET S(28, 74)
#define PASSED_PAWN_OFFSET S(-17, -8)
#define BISHOP_PAIR S(26, 43)
#define DOUBLED_PAWN S(5, 15)
#define TEMPO S(21, 21)
#define ISOLATED_PAWN S(9, 9)
#define PROTECTED_PAWN S(7, 4)
#define ROOK_OPEN S(22, 3)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-41, -3)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(13, -6)
