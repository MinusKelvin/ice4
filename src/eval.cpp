// loss: 0.07981    train id: 2024-01-12-23-59-21-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int PIECE_RANK[] = {S(0, 0), S(20, 35), S(24, 39), S(25, 40), S(30, 44), S(38, 57), S(29, 100), S(0, 0), S(113, 118), S(121, 131), S(130, 147), S(134, 158), S(132, 166), S(158, 149), S(137, 144), S(36, 145), S(117, 132), S(119, 131), S(120, 139), S(114, 138), S(113, 140), S(123, 139), S(103, 143), S(68, 158), S(168, 260), S(157, 260), S(158, 266), S(159, 276), S(173, 279), S(192, 277), S(191, 287), S(199, 286), S(313, 497), S(309, 498), S(300, 533), S(288, 561), S(284, 578), S(296, 580), S(282, 593), S(291, 575), S(-10, -53), S(18, -5), S(-7, 10), S(-34, 21), S(-25, 29), S(14, 34), S(18, 25), S(-2, -42)};
int PIECE_FILE[] = {S(13, 56), S(24, 53), S(33, 47), S(42, 38), S(44, 41), S(49, 46), S(45, 46), S(23, 44), S(98, 136), S(111, 148), S(116, 159), S(124, 162), S(124, 163), S(125, 157), S(116, 152), S(108, 137), S(128, 161), S(130, 161), S(130, 158), S(126, 160), S(129, 157), S(127, 162), S(132, 163), S(131, 160), S(124, 271), S(127, 273), S(136, 272), S(140, 269), S(145, 262), S(137, 267), S(137, 266), S(133, 261), S(331, 525), S(326, 540), S(324, 551), S(326, 557), S(324, 559), S(325, 561), S(331, 556), S(336, 558), S(-19, -39), S(21, 3), S(4, 13), S(-28, 21), S(-16, 18), S(-20, 17), S(21, 3), S(-17, -38)};
int MOBILITY[] = {0, S(4, 9), S(2, 3), S(5, 7), S(4, 4), S(3, 3), S(-15, -8)};
int PASSED_PAWN[] = {S(0, 0), S(-2, 2), S(-9, 11), S(-6, 30), S(5, 51), S(11, 89), S(29, 100), S(0, 0)};

#define ISOLATED_PAWN S(-10, -11)
#define DOUBLED_PAWN S(-7, -10)
#define KING_ATTACKS S(11, -8)
#define DOUBLE_KING_ATTACKS S(26, -2)
