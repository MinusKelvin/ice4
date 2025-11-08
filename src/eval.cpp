// loss: 0.07821    train id: 2025-11-08-18-34-48-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(12, 74), S(126, 340), S(176, 380), S(274, 687), S(577, 1328), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(19, 0), S(28, 0), S(25, 0), S(26, 0), 0};

#define DATA_STRING L"7RN973- :H=862&\"<DMJG<-'BNVXPB<.NVxgc^I;,'KaVDC9jv¥  -4:==5-KVXUQXK  )(#'$.&-!$ *754 #*0611,@GB:581  !\"#$%)'`Y=% :;?GJ9 1-QO *-3'$'%$ # $/1_&% \"&+6/Y XX^acWWefsÅ  (05'+),$&//-(((*/&%\"'.-87+$\"+3;LX:( .CDeh`428>E 7O\\bN?;#8MQOH:  \"12522?&**0,0+  \"(388DD123-&*% \" BZfirg -8>=<52 '5BSa[)&?KTKK; 9  \"* '$%! 3V~ºò UF4+---F B^lw} %4V°ȓ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(8, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -24)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-12, -59)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-6, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-57, -38)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-55, 5)
#define PAWN_ADVANTAGE S(14, 12)
#define ATTACKER_NO_QUEEN S(-96, 0)
