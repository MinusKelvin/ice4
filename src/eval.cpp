// loss: 0.05072    train id: 2025-10-02-08-36-35-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(7, 90), S(128, 425), S(178, 451), S(294, 832), S(677, 1481), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(49, 0), S(19, 0), S(33, 0), S(30, 0), S(29, 0), 0};

#define DATA_STRING L"SvjWFG@/XeRTNH6,V]eh`PB3bjv{gYP<u®\\N Cc{lMI1cu¶  7BMPRE:[gic_dY  .,$*)6+2*) *85#$ (184A3_f^XNSG  \"!$#(1.osI& SbbXgL >3hd /5@+',(&  \"$5,p/$ )05I5a ^^gjr^Óãìþĩ -6BK5;>B26?=;4:><B5116@DPL8-/:IXswG$ ;ginp7.CRe#;UcfO8 +;OTOH9  %98:5.9+-*402* ! $498LI7<?5,2&  !BQ^]kg (6<A=41 -CVisl%%EZj\\\\C I)& ,!'#(  7dòĹ gU=1319F Dgt 0<eûȲ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 81)
#define TEMPO S(21, 32)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(18, 20)
#define ROOK_OPEN S(34, 11)
#define ROOK_SEMIOPEN S(12, 14)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-18, -32)
#define KING_OPEN S(-67, 0)
#define KING_SEMIOPEN S(-12, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-6, 1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-17, -80)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-15, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-65, -38)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-178, -1)
#define ATTACKER_NO_QUEEN S(-86, 0)
