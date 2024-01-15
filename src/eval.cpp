// loss: 0.07918    train id: 2024-01-15-14-12-18-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int PIECE_RANK[] = {S(0, 0), S(19, 35), S(27, 38), S(29, 39), S(35, 45), S(45, 58), S(35, 106), S(0, 0), S(105, 120), S(117, 139), S(130, 162), S(134, 173), S(137, 179), S(149, 166), S(125, 154), S(32, 146), S(113, 139), S(123, 140), S(130, 151), S(127, 152), S(125, 154), S(131, 152), S(105, 155), S(72, 166), S(187, 274), S(174, 273), S(177, 276), S(175, 285), S(187, 287), S(207, 285), S(209, 293), S(208, 297), S(327, 509), S(332, 506), S(327, 541), S(318, 566), S(316, 580), S(326, 582), S(315, 593), S(314, 575), S(10, -38), S(2, 1), S(-17, 10), S(-38, 18), S(-33, 28), S(1, 37), S(5, 30), S(15, -28)};
int PIECE_FILE[] = {S(26, 54), S(30, 54), S(36, 49), S(44, 41), S(46, 45), S(51, 49), S(48, 49), S(29, 45), S(106, 145), S(121, 163), S(128, 181), S(135, 184), S(137, 183), S(137, 177), S(128, 166), S(117, 144), S(117, 164), S(126, 168), S(127, 166), S(125, 170), S(128, 167), S(122, 171), S(127, 169), S(121, 161), S(141, 285), S(144, 286), S(154, 286), S(160, 280), S(166, 274), S(156, 279), S(153, 278), S(149, 274), S(339, 536), S(339, 551), S(341, 559), S(342, 566), S(342, 568), S(343, 569), S(348, 563), S(348, 565), S(10, -30), S(16, 3), S(-4, 14), S(-34, 22), S(-23, 18), S(-25, 17), S(16, 2), S(12, -31)};
int MOBILITY[] = {0, S(3, 7), S(1, -2), S(4, 5), S(2, 3), S(3, 2), S(-4, -5)};
int PASSED_PAWN[] = {S(0, 0), S(19, -15), S(8, -3), S(8, 21), S(16, 49), S(16, 93), S(35, 106), S(0, 0)};
int THREATENED[] = {0, S(0, 0), S(-35, -12), S(-30, -32), S(-41, -20), S(-40, -29), S(0, 0)};

#define ISOLATED_PAWN S(-9, -11)
#define DOUBLED_PAWN S(-12, -12)
#define KING_ATTACKS S(13, -8)
#define DOUBLE_KING_ATTACKS S(27, -1)
#define TEMPO S(15, 14)
#define ROOK_BEHIND_PAWN S(-16, -11)
#define BISHOP_PAIR S(22, 47)
#define PASSER_ATTACKED S(14, -26)
#define PASSER_OPP_KING_DIST S(-8, 9)
