// loss: 0.05212    train id: 2025-10-03-18-02-52-sirius-mixed

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(13, 88), S(99, 357), S(151, 395), S(232, 695), S(615, 1233), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(37, 0), S(23, 0), S(26, 0), S(20, 0), S(25, 0), 0};

#define DATA_STRING L"3QO7872&8H<;87**<AMLL?1,DOVWRIA6P]qj`\\LC5 JY[B=Ibnz£  .7>@C:1FUWTRY=  '%\"$#-$2 *'1;=S \"*47;-'4:3/*,$  %(&'++(hd?% 4s³e^J >6df .5?#\"$#  #\" *3['# $%,8(\\ `^`bdR +.8pÚ (59,.**##50/*,%,3-*$(2-;=4*)(7;KOD-\"6CG^kc>/,BP 8Ucj[J4(ATZYO@  &6<@:=@&02646-  %*9@FVE/11*&'%  %Ni{~ )1@B=97 +BM]lk4!?L\\JR@ 8#! + &%(  .Ux³ÿ UJ2*++4B AZgnqw -5T°ſ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(18, 66)
#define TEMPO S(9, 7)
#define ISOLATED_PAWN S(5, 10)
#define PROTECTED_PAWN S(10, 15)
#define ROOK_OPEN S(30, 2)
#define ROOK_SEMIOPEN S(12, 8)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-16, -24)
#define KING_OPEN S(-42, -1)
#define KING_SEMIOPEN S(-11, 9)
#define MOBILITY_INDEX 131
#define MOBILITY S(0, -2)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-11, -55)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-60, -34)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(0, -1)
#define ATTACKER_NO_QUEEN S(-63, 0)
