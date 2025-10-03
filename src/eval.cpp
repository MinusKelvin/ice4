// loss: 0.05156    train id: 2025-10-03-20-27-56-sirius-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(20, 99), S(117, 427), S(174, 458), S(273, 827), S(655, 1524), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(46, 0), S(24, 0), S(32, 0), S(26, 0), S(29, 0), 0};

#define DATA_STRING L"8ZU<860!<L??;7'\"?EQRN>1'HUabVIA0Ye|ojMC$ ReeB5@fuµ  3>GKMB7M\\]XU\\J  ,+#''4)1 '\"-;<K! (3974-AF>705+  #$%$*/+rqG% IvjlU D9oo 06A'$(&$ %# -+f+% &)1A-b fdhhlW -4Ak2#.:?2324),944-102:0+(.77FD4()0@KafE' 8YY{vo<,8P[ :Zlp[C)+CY^\\Q@  '=@C=7=+11967.  #)8>@RE7;=4.2(  &Sm\177}\177 +;EJC74 /FVhsk+\"BSgTZB D'$ - '%'! 4dçķ cS9./.7J Fgv .;bíș"

#define EG_OFFSET 166

#define BISHOP_PAIR S(20, 81)
#define TEMPO S(14, 22)
#define ISOLATED_PAWN S(6, 10)
#define PROTECTED_PAWN S(14, 17)
#define ROOK_OPEN S(34, 7)
#define ROOK_SEMIOPEN S(13, 12)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-18, -29)
#define KING_OPEN S(-54, -1)
#define KING_SEMIOPEN S(-13, 11)
#define MOBILITY_INDEX 131
#define MOBILITY S(-3, -1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-13, -74)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-11, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-66, -42)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(1, -2)
#define ATTACKER_NO_QUEEN S(-79, 0)
