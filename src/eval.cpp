// loss: 0.02942    train id: 2025-10-03-20-34-57-sirius-mixed

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(16, 100), S(124, 408), S(185, 450), S(281, 793), S(717, 1432), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(43, 0), S(26, 0), S(29, 0), S(24, 0), S(29, 0), 0};

#define DATA_STRING L";]Y<=<6(?QB@=<-,CJWUTE60MYbc\\QG<\\hwmhUJ; OcgJBPky¸  0;BEG=2J[^[X`A  )'\"$#0%2 )&2?AY \",6:>0)4;4.)+\"  %(&(,-)urF% :r¸fcL >7ij 19C%#&$\" %\" ,8f)$ &(/<,d hgikmY .1=ë\"*7=.1-.%$721,/(.5-+$)51>A5*)*;@PWH, 6GLj|tI41JV <\\ltbN9(CX_^SC  '9?D>@D%13868.  %+=DJ]K144,()&  (Xx ,7HJD>8 +EShz{9\"CQaOWC 9! !-!'&)  1aÐġ ^P5++,5J Geu~ .8]Âǋ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(21, 74)
#define TEMPO S(14, 10)
#define ISOLATED_PAWN S(6, 11)
#define PROTECTED_PAWN S(13, 17)
#define ROOK_OPEN S(35, 2)
#define ROOK_SEMIOPEN S(14, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-18, -27)
#define KING_OPEN S(-49, -1)
#define KING_SEMIOPEN S(-13, 10)
#define MOBILITY_INDEX 131
#define MOBILITY S(-1, -3)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-13, -68)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-9, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-68, -42)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(0, -1)
#define ATTACKER_NO_QUEEN S(-73, 0)
