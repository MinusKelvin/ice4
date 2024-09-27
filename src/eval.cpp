// loss: 0.07868    train id: 2024-09-27-23-34-34-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(23, 69), S(139, 265), S(189, 302), S(300, 552), S(657, 1042), S(0, 0)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(4, 5), S(2, 2), S(1, 4), S(-3, 0)};

#define DATA_STRING L"9SP:84.\"<I>973'$>ENKI=/)DPXYRD>1R[{iebO?( DVM=?7lx£  -4:<=5,MXZWS[M  ))$'%.&,!# +:9; #*0610+4;6-(1'  !\"##%)&`\\= \"O^pEJ9 1-QN ),2# %01h]_WX\\boi] _]bedX9GIV²  '.3&*(*$%--,''&(,%$!&,*43)#\")/5CL5'!+:;VYT3256; 5JT\\J=9#5HLJD7  !./2/0;%)(-*-(   &/21::,-.*#&# # @Wegpb ,6=>>8:(1=FPWN %8@HAA4 :\" \" 0MmË>f\\MFHGF> ;Q]flp $2LƟ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-11, -19)
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-10, 11)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-12, -48)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-122, -60)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-25, 4)
