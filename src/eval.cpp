// loss: 0.0791    train id: 2024-09-10-18-36-01-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 234), S(229, 238), S(224, 233), S(224, 240),
    S(236, 301), S(234, 306), S(238, 302), S(235, 305),
    S(291, 559), S(304, 576), S(298, 551), S(322, 565),
    S(607, 1095), S(593, 1131), S(610, 1095), S(605, 1142),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -22), S(6, -17), S(14, -9)};
int MOBILITY[] = {0, S(5, 10), S(1, -1), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PASSER_RANK[] = {S(5, 6), S(-3, 15), S(-3, 34), S(8, 56), S(12, 105), S(59, 139)};

#define DATA_STRING L"[trYVUQFdria_]RMckurpeWPgs}}vjcXrytf= ^~sedXA@]C #3D4/'Wu^f )/2+2@>6FHO=;NM!$ $'20))74-')*7/27? +32)4/1\"&*+ ),0./44231,-*%$)09?3659-09:63575:31/4<<AA7--7@GNW;$ 0FLstd65?OX#3/ 4LH5IYYL9=E4 Lfk]mvzf|p -**$(+,1244+352!#' '''&\"#&%'*)'% \",*$)/.=FB7GOT"

#define PAWN_OFFSET S(-12, 61)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(9, 10)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 14)
#define KING_RING_ATTACKS S(13, -5)
