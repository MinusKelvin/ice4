// loss: 0.05156    train id: 2025-10-03-20-27-56-sirius-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(18, 86), S(102, 374), S(153, 401), S(238, 724), S(573, 1333), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(21, 0), S(28, 0), S(23, 0), S(26, 0), 0};

#define DATA_STRING L"5SN853.!8F:;74&!:@KKH:/&CNXZOC<.Q\\|pe`G># K\\\\=2<]kx£  1:BFG=4GUUQNUE  *)\"&%0(/ &\",89F! (1652,<A:4.2*  #$%$)-*hgB$ DkzacN @6ee /3=%#&$# $\" +*])$ $'.=*Z ^\\__bP +2=b0#-8<0102(+722,/.17.*(-55B@2()/<FY^A' 5RRple9+6KT 7SbeS>')?RVTK<  &9<?:4:)//634,  #(5;<LA4791,/&  %Mcsq{s +8AE?52 -AP_ib*!>M^NS> ?&# + &$&! 2[ÏĔ [L6,-,4E B^kv{} -8ZÔǚ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(17, 71)
#define TEMPO S(12, 19)
#define ISOLATED_PAWN S(5, 9)
#define PROTECTED_PAWN S(12, 15)
#define ROOK_OPEN S(30, 6)
#define ROOK_SEMIOPEN S(12, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-16, -25)
#define KING_OPEN S(-48, -1)
#define KING_SEMIOPEN S(-11, 10)
#define MOBILITY_INDEX 131
#define MOBILITY S(-2, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-11, -65)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-9, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -37)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(1, -2)
#define ATTACKER_NO_QUEEN S(-69, 0)
