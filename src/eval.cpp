// loss: 0.07903    train id: 2024-01-09-14-12-58-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 234), S(228, 239), S(223, 233), S(223, 241),
    S(235, 302), S(232, 307), S(237, 303), S(233, 306),
    S(290, 557), S(303, 575), S(297, 550), S(320, 564),
    S(608, 1094), S(594, 1131), S(611, 1094), S(605, 1144),
};
int PAWN_SHIELD[] = {S(-3, -8), S(3, -21), S(5, -16), S(13, -11)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"8QO621- BPG?<;.(@ISOLB3*FR\\ZRF?2Vc|d``J=B-cfIn1H>9//5635)+'',7,8)\"**=B*5758BKE  6ADIUPAUL@D[v=V??]D #4H62-\\s_j )/2+2@>6FHO=;NM\"$ $'30))75-')*7/27? +32)4/0#&*, (,/-.33120,,)$$ *8B85//#(6:90.+)0/00250474259:<BVAQSHPD\177  §«¤0-% %(3-4@5-'.:3SWLA69BHqqaMFIVb ZSlzv@@BL]\"2/ 2JG3EXWH6=C0 Lfk]luze{p -*)$(*+0034*353!#' ''&%\"#&$'*)'% \",*$)/.=FB7GOU"

#define PAWN_OFFSET S(2, 68)
#define PASSED_PAWN_OFFSET S(-14, -8)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(6, 15)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 8)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 11)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
#define PAWN_ADVANTAGE S(4, 3)
