// loss: 0.07905    train id: 2024-01-08-11-36-52-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 235), S(229, 240), S(224, 233), S(224, 242),
    S(236, 302), S(233, 308), S(238, 303), S(234, 307),
    S(292, 557), S(305, 574), S(298, 550), S(322, 563),
    S(608, 1096), S(594, 1134), S(611, 1096), S(606, 1146),
};
int PAWN_SHIELD[] = {S(-3, -7), S(3, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7PN521- AOF><:.(?HRNLA3*FR[ZRF?2Ub{c`_I=?*`cFk2F:8//3344'*'',5,7(\"++=@,898<DND $7CFKWOCYOAF]x<V?>\\D \"3J82,`zdm )02+2@>6FHO=;NM\"$ $(30))75.')*7/27? +32)4/0#&*+ (,/-.33131,,*%$ +9D:700#(6;;2/,)101246137436;;<AUARTIPD¡¢¨«¦2-% %(4/5?5-'/;6UVLA7;DLsrbNHKYf¤\\Uo~£¡xCBEQb\"2/ 2JF3DWWH5<C/ Kfj\\luye{p -*)$(*+0034*352!$' '''%\"#&$'**'% \",*#(/.<EA7FNT"

#define PAWN_OFFSET S(25, 72)
#define PASSED_PAWN_OFFSET S(-20, -10)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 13)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 5)
#define ROOK_SEMIOPEN S(12, 7)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
#define OWN_ROOK_BEHIND_PASSER S(11, 4)
#define OPP_ROOK_BEHIND_PASSER S(-29, -23)
