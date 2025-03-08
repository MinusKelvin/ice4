// loss: 0.07826    train id: 2025-03-09-09-12-02-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(23, 81), S(127, 343), S(177, 384), S(272, 695), S(582, 1341), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(32, 0), S(15, 0), S(24, 0), S(16, 0), S(27, 0), 0};

#define DATA_STRING L"7QM862, :G<851%!;CLIG;-&AMVWOA<-NWxgc^I:&!E[P=<2it¦  -4:=>6-KVXUQZK  )(#&$.&-!# *8:: #*0622,AGB:570  !\"##%)'`W;& 89<GJ9 1-QP *-4'$'%$ # $01c'$ \"',71Z ZY_bdW\\jkwË !(05'+*-%&//-(((*/&%\"'/.87+$\",3;LW:' .CEdf^207=D 8P\\aM>9$9MROH:  \"002/0<'+*1-1+  !'265??233-%)% # BZfjsg -7><;53 '7DUb],%?JTKK: ;  \"* &$%\" 4X\177¼ö UG5,.--D @^kv} %4U¯Ț"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 55)
#define TEMPO S(10, 15)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-14, -26)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -62)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-58, -36)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-60, 5)
