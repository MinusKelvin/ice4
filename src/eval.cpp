// loss: 0.02982    train id: 2025-10-03-20-48-47-sirius-mixed

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(15, 90), S(112, 367), S(168, 403), S(254, 712), S(649, 1283), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(38, 0), S(23, 0), S(26, 0), S(21, 0), S(26, 0), 0};

#define DATA_STRING L"8VS9994';L><:9++?EQOOA3.HS[\\ULC9Vawne`OF7 J\\_E=Jdq~©  /8?AC:0FUWURY=  (&!##/%1 )&0<>S \"*47:.(182,()!  %(&'+,)khA$ 7i¨_\\H ;5bb /6?%\"%$\" %\" +6`($ %'.9+] a_bceS -0:yÚ!)4:,/+,$$50/+-',3+)$(2/;=2(')8=KQC* 4CGbqjC0.DP 9Uck[I6&>QXVM?  &7<@;=A%01645,  $*:@EWF.11*&'%  'Ro +4CE@:5 *BOarr6!@LZJQ@ 6! !+!&%'  0[¾ć WK3**+3F C^ltx~ -6W²ƞ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(19, 67)
#define TEMPO S(13, 9)
#define ISOLATED_PAWN S(6, 10)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(31, 2)
#define ROOK_SEMIOPEN S(12, 9)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-16, -24)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-11, 9)
#define MOBILITY_INDEX 131
#define MOBILITY S(-1, -2)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-12, -61)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-8, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-61, -38)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(0, -1)
#define ATTACKER_NO_QUEEN S(-64, 0)
