// loss: 0.0792    train id: 2024-01-13-16-00-24-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int PIECE_RANK[] = {S(0, 0), S(21, 36), S(26, 39), S(27, 40), S(32, 46), S(42, 59), S(32, 103), S(0, 0), S(113, 121), S(125, 135), S(133, 151), S(138, 161), S(135, 169), S(148, 155), S(128, 149), S(38, 146), S(109, 132), S(115, 132), S(117, 142), S(112, 142), S(111, 143), S(119, 142), S(92, 148), S(62, 161), S(180, 274), S(168, 271), S(167, 277), S(167, 286), S(179, 289), S(197, 288), S(197, 298), S(204, 296), S(321, 502), S(322, 502), S(314, 537), S(304, 564), S(302, 580), S(315, 583), S(302, 595), S(307, 577), S(-10, -52), S(14, -3), S(-4, 10), S(-27, 20), S(-19, 28), S(19, 34), S(23, 24), S(14, -46)};
int PIECE_FILE[] = {S(20, 55), S(27, 54), S(35, 48), S(45, 39), S(42, 45), S(54, 46), S(44, 47), S(28, 44), S(99, 140), S(113, 151), S(117, 164), S(125, 167), S(125, 167), S(127, 161), S(119, 156), S(109, 140), S(117, 161), S(120, 162), S(120, 159), S(117, 161), S(119, 159), S(117, 164), S(123, 165), S(121, 159), S(134, 285), S(137, 286), S(147, 285), S(152, 280), S(157, 274), S(150, 280), S(146, 278), S(144, 273), S(339, 527), S(335, 542), S(334, 551), S(335, 557), S(334, 559), S(334, 561), S(341, 556), S(344, 558), S(-12, -39), S(19, 4), S(1, 13), S(-29, 21), S(-16, 17), S(-21, 17), S(20, 3), S(-11, -39)};
int MOBILITY[] = {0, S(3, 8), S(2, 2), S(5, 6), S(3, 3), S(3, 3), S(-13, -8)};
int PASSED_PAWN[] = {S(0, 0), S(-4, 2), S(-11, 10), S(-7, 30), S(6, 52), S(11, 90), S(32, 103), S(0, 0)};
int THREATENED[] = {0, S(0, 0), S(-33, -11), S(-29, -31), S(-39, -22), S(-41, -27), S(0, 0)};

#define ISOLATED_PAWN S(-9, -11)
#define DOUBLED_PAWN S(-10, -11)
#define KING_ATTACKS S(10, -8)
#define DOUBLE_KING_ATTACKS S(29, 0)
#define TEMPO S(15, 13)
#define ROOK_BEHIND_PAWN S(-12, -17)
#define KING_BEHIND_PAWN S(24, -7)
#define BISHOP_PAIR S(23, 48)
