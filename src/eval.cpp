// loss: 0.11527    train id: 2024-01-08-16-49-53-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(232, 243), S(245, 248), S(239, 241), S(239, 250),
    S(253, 309), S(251, 314), S(255, 311), S(251, 313),
    S(315, 574), S(327, 593), S(321, 567), S(345, 582),
    S(616, 1157), S(602, 1194), S(619, 1158), S(614, 1202),
};
int PAWN_SHIELD[] = {S(0, -12), S(5, -22), S(7, -16), S(14, -13)};
int MOBILITY[] = {0, S(5, 7), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"8TR723. BSI@><0(@KUQND5+GU^\\THB3Wf\177febK>>(goLo0C87,+0530%(##)6-5& ((<C-797:DNH#$:EEM[TH\\LBJ[}CW><\\D #3H=9.Zwbi )24,3AA7IJR@>QO#$ $(41*)85-()*7149A ,43)602$(+- )-0./44232,-+%$ )7B95./#'5990-+)0//02402520499:@R=PRFMAzz0,% &(5-2>2,&-:2RVKA68BHtsbMFGWd§YRj~«¬QNQ[s!41 2LI5G[[L;CK8 Ofl^mtyezp 0-,%)+-2366+586\"$' )()'#$'&(**(' #,& %,)8@<3@HN"

#define PAWN_OFFSET S(28, 74)
#define PASSED_PAWN_OFFSET S(-17, -8)
#define BISHOP_PAIR S(24, 47)
#define DOUBLED_PAWN S(5, 13)
#define TEMPO S(11, 13)
#define ISOLATED_PAWN S(9, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(11, 9)
#define KING_OPEN S(-45, -3)
#define KING_SEMIOPEN S(-9, 15)
#define KING_RING_ATTACKS S(14, -6)
