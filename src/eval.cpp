// loss: 0.07873    train id: 2024-09-20-18-09-44-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 237), S(230, 241), S(224, 235), S(226, 243),
    S(235, 307), S(233, 313), S(237, 308), S(234, 312),
    S(292, 566), S(305, 583), S(299, 558), S(323, 571),
    S(609, 1107), S(596, 1143), S(612, 1107), S(608, 1155),
};
int MOBILITY[] = {0, S(4, 7), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"<VS;871$@LB<97*'BIRNK@2,HT\\]UGA4V^~lhcQC, EXO=?;_HGeN&*<K' *VrVa )02+2@>6FHO=;NM\"$ $(30))74-')*7/28? ,32)4/1#'+, ),/./44131,,*%$ *,3$ $01iYYSTX_kbY [Y^``V4BDP¬  '04')&)\"$--*$%$'+$# $*(33)\"!(.4BK5% *9;QTP.-226#0, *=:-@LKD:8?4 Lgl^mvze|p -)*%)++0144*353\"#& '&&%\"\"&%')(&& \"-)$)/.=FB7GOT8  % 0Mn Ð>i[LFGG?> ;Q]fkv #1Kƞ"

#define EG_OFFSET 160

#define PAWN_OFFSET S(19, 72)
#define BISHOP_PAIR S(24, 45)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(7, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-6, -16)
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-10, 10)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-12, -48)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-114, -60)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-20, 5)
