// loss: 0.07866    train id: 2024-10-28-10-30-39-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(18, 69), S(138, 265), S(189, 303), S(300, 553), S(657, 1044), S(0, 0)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(4, 5), S(2, 2), S(1, 4), S(-4, 0)};

#define DATA_STRING L">XU?=93'ANC><8,)CJSPNB4.IU]_WIC6O[whecPA- HYPBB=mx¤  -4:<=5,MXZWS[M  ))$'%.&, \" *:9; #*0610+4;6-(1'  !\"##%)&a^> !L]oEJ9 1-QN4<>D ,3;# %04f\\^WW[bnh\\ ]\\addW2@BO¦  &-2&*(+$%--+''&(-%$!&,*44)#\")/5EQ;,%,<<W\\W6478< 5JT\\J=9#6HLJD8  \"./2//;%(',),' ! &/21:;,--)\"&\" \" ?Vefob +6<=>8:+1>FQYM %7AHBA4 @113R*  0Ll¢Î>f\\NGHHG> >Q\\eko $2LƖ"

#define EG_OFFSET 164

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-28, -39)
#define KING_OPEN S(-45, -1)
#define KING_SEMIOPEN S(-10, 10)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 135
#define PASSER_RANK S(-12, -48)
#define KING_PASSER_DIST_INDEX 142
#define KING_PASSER_DIST S(-120, -60)
#define PHALANX_RANK_INDEX 157
#define PHALANX_RANK S(-18, 4)
