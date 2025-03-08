// loss: 0.07816    train id: 2025-03-09-09-18-00-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(22, 85), S(122, 384), S(172, 427), S(260, 766), S(565, 1476), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(15, 0), S(24, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7RM862, :G<851%!<CLIG;-&ANVXPA<-MWyhd]H:'#G^R>;1it\177¥  ,39<=5,ITVSPXI  )(#&$.&-!$ *789 \"*0612-CJE=892  !\"$$&*'cZ=' 4.-IK: 2-RR ),3'$'%$ # $/1b'# \"',7/Z XX^bdWfsuÒ \"*27)-,/&'110***-1(($*21<:-&$.6?R]<( 1HKmoe319BK 9R`eP@;#:OTQJ;  !10301=',+2.3,  \"(499DD455/&+& # BZglvn -8>=<64 (:J^oo;&CP[QQ> ;  #* '$%! 5\\ÉĈ [J6,../H Ddr~ &6Z¿ɕ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 59)
#define TEMPO S(10, 16)
#define ISOLATED_PAWN S(7, 9)
#define PROTECTED_PAWN S(11, 18)
#define ROOK_OPEN S(23, 6)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-13, -28)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -66)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -40)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-70, 5)
