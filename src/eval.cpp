// loss: 0.07867    train id: 2024-10-28-17-25-57-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(24, 69), S(138, 264), S(189, 301), S(300, 551), S(657, 1043), S(0, 0)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(4, 5), S(2, 3), S(1, 4), S(-3, -2)};

#define DATA_STRING L"7QM863-!:F;762'#=ENJH<.(DOXYQC=0Q[zhd`M>' DUL<>6my¥  -4:<=5,MXZWS[M  ))$'&/&-!# +:9: #*0510+4;6-(1'  !\"##%)&gb?  O_rHI9 1-PP -3;# %02h]^WX\\boi] ^]bdeX9GIV´  '/3')')%(0/,&&&'+$# %+*32(\"!(.4BK5'!+:;UYS3256; 5KU]K>:#6HLJD7  !/03/1=%)(-)-( ! &032:;,-.)#&# \" @Xfgpc +6<==8:#=@JV\\T $>FNGG; B+#  0MnË>f\\MFHHF> ;Q]fkp $1MƟ"

#define EG_OFFSET 160

#define BISHOP_PAIR S(22, 47)
#define TEMPO S(10, 13)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 14)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-13, -27)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-9, 9)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-12, -48)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-122, -60)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(-25, 4)
