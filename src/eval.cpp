// loss: 0.0507    train id: 2025-10-02-08-39-15-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(3, 81), S(107, 376), S(148, 400), S(244, 737), S(507, 1383), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(43, 0), S(17, 0), S(29, 0), S(26, 0), S(25, 0), 0};

#define DATA_STRING L"NlbRDD>/R^NOJE5-PW]`YK@2[bmp_TL:kx|yVJ B[pdJG2Xgx\177  4=GJK@7R]_YV[R  -,$)(3)0)( )52## '.40<0X]VOGKC !\" #\"&/-fjD% NYZP]E 90^[ -2;*&*'&  \"#1)a-# '.2C2W VU]^eTĊėğĮŐ )2<D058;.2:76/587<1-,1:=HE3*+5AOhkA\" 6]^}bf3,=JZ$9Q^aL8 )8JMJD5  $5462+5*+)2.0) ! $377GF49;3+0&  \"EUa^hf +:@D>3- +?O_hb$$@R`TT? D(% + &#%  3\\Ùę _O9/1/5C A`kw{z -9^åȨ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(19, 70)
#define TEMPO S(18, 29)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(15, 17)
#define ROOK_OPEN S(29, 10)
#define ROOK_SEMIOPEN S(11, 12)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-15, -28)
#define KING_OPEN S(-57, 0)
#define KING_SEMIOPEN S(-11, 10)
#define MOBILITY_INDEX 131
#define MOBILITY S(-5, 1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-14, -70)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-13, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-55, -35)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-233, -1)
#define ATTACKER_NO_QUEEN S(-81, 0)
