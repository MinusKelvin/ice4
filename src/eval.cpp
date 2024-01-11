// loss: 0.08044    train id: 2024-01-11-17-18-00-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int PIECE_RANK[] = {S(0, 0), S(19, 32), S(24, 36), S(24, 37), S(26, 49), S(39, 102), S(62, 171), S(0, 0), S(110, 115), S(117, 131), S(125, 149), S(129, 160), S(134, 164), S(162, 148), S(141, 142), S(41, 142), S(114, 128), S(116, 129), S(118, 137), S(112, 135), S(113, 136), S(127, 134), S(105, 139), S(69, 154), S(162, 258), S(147, 257), S(148, 262), S(148, 272), S(160, 276), S(182, 273), S(190, 278), S(194, 278), S(306, 486), S(302, 488), S(294, 524), S(284, 551), S(284, 563), S(301, 562), S(287, 575), S(297, 556), S(-9, -56), S(25, -13), S(-17, 9), S(-49, 24), S(-40, 33), S(10, 36), S(19, 25), S(-12, -38)};
int PIECE_FILE[] = {S(5, 69), S(27, 71), S(30, 59), S(39, 54), S(43, 53), S(46, 55), S(51, 62), S(17, 55), S(96, 133), S(107, 146), S(111, 160), S(119, 163), S(121, 162), S(122, 157), S(116, 150), S(108, 134), S(125, 156), S(126, 156), S(126, 152), S(122, 154), S(124, 151), S(122, 157), S(128, 159), S(130, 153), S(121, 269), S(126, 272), S(138, 271), S(143, 268), S(149, 260), S(140, 265), S(140, 262), S(134, 256), S(326, 506), S(321, 523), S(319, 534), S(320, 540), S(318, 542), S(321, 546), S(328, 540), S(336, 541), S(-26, -36), S(22, 0), S(5, 12), S(-27, 19), S(-14, 16), S(-20, 16), S(23, 1), S(-25, -33)};
int MOBILITY[] = {0, S(5, 11), S(3, 1), S(6, 6), S(5, 3), S(4, 3), S(-21, -4)};

