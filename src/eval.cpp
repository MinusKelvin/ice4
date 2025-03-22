// loss: 0.07823    train id: 2025-03-22-20-14-33-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(22, 81), S(127, 344), S(177, 384), S(273, 696), S(579, 1344), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(35, 0), S(21, 0), S(27, 0), S(18, 0), S(31, 0), 0};

#define DATA_STRING L"7QM973- :H=962'#<DMJH<.'BNWYPB=.OYzje_J;($H^R?=3ju¦  -4:=>6-KVXUR[L  ))$'%.'-!$ *8:; \"*0622,AGC;660  \"#$$&*)YS5$ 799HI9 1,PP ),4&#&$# # %12e'  \"'+60[ XY_cfX[giuË  (04'+*-$&..-(((*/&%\"'/.87+$\",3;LV9' .CEfh`439?F 8P[`L=9#8MQNH:  \"002.0<'++1-1,  !'265??123,%)$ \" C[eirh -8>=<52 ,<HYfa2&=ISJI9 ;  #* &$%\" 3W\177¼ó RC1(*))D @^kv| %4U¯Ț"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 17)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-7, -20)
#define KING_OPEN S(-43, -2)
#define KING_SEMIOPEN S(-9, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-2, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -57)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-59, -36)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-58, 5)
#define DEFENDER_PAWNS S(-14, 0)
