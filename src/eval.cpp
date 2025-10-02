// loss: 0.0507    train id: 2025-10-02-08-39-15-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(3, 69), S(92, 322), S(128, 343), S(208, 632), S(434, 1185), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(37, 0), S(15, 0), S(25, 0), S(22, 0), S(22, 0), 0};

#define DATA_STRING L"GaXJ>>9-JUGHC?2*INTWPE;/RXaeVLE6`kxokNC =RdYCA/Q^lr{{  08ACD:3KTUQNRJ  *)#'&0'/(' (20## '-2/9/PUNHAE> !\" #\"&-,\\_?$ GPRIT@ 6.VS +/8(%(&%  \"#/'X+# &,/>/O NNTU[MéôûĈĥ (08?.358,0643.2549/,+/7:C@1)*2=I^a=# 4UVpY\\1+:DR#5JUWF4 (5DGD?3  $2240*2()'.,-'   #033B@2570*.%  \"@NXV^] *7<?:1, *;IW^Y$$<KXMM; >'$ ) %\"$  1Sx¾ö VH6-.-2> <Vaknm ,5UÉǞ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(16, 60)
#define TEMPO S(15, 24)
#define ISOLATED_PAWN S(6, 7)
#define PROTECTED_PAWN S(13, 15)
#define ROOK_OPEN S(25, 9)
#define ROOK_SEMIOPEN S(9, 10)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-13, -24)
#define KING_OPEN S(-49, 0)
#define KING_SEMIOPEN S(-9, 9)
#define MOBILITY_INDEX 131
#define MOBILITY S(-4, 1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-12, -60)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-11, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-47, -30)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-200, -1)
#define ATTACKER_NO_QUEEN S(-69, 0)
