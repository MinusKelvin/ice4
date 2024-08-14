// loss: 0.07909    train id: 2024-08-15-09-11-37-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(212, 235), S(224, 240), S(219, 234), S(219, 242),
    S(230, 304), S(227, 310), S(232, 306), S(228, 309),
    S(232, 540), S(245, 559), S(239, 532), S(262, 548),
    S(510, 1099), S(496, 1135), S(513, 1100), S(507, 1148),
};
int PAWN_SHIELD[] = {S(-2, -7), S(4, -21), S(5, -16), S(13, -9)};
int MOBILITY[] = {0, S(5, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"7ON521, AME=<:-(?GQNLA2*EPYYQE>2U`yb_^H<E)`eFp3E:7./3742'*'',8.6(#+*=D,587;CLG! 5CEIVQ?SK?CYu;V@?]E #4J61-dzdo )/2*2@>6FHO=;NL\"$ $'30))75-')*7/28? +32)4/0\"%)+ (,/-.33120+,)$# (8C96.0$'5;;1.-*//2335244337::<@R>QTGNC{£¥©©¦2.% %)5/5@5.(0<5TWM@6:DJrtcNGJXe£\\Uo~¥w?=CP^\"3/ 3JG4EXWH5<C/ Mfl]mvzg|q -**$(*+1144*353!$' (((&#$'&)++(% \",*$)/.=FA7FNS"

#define PAWN_OFFSET S(49, 70)
#define PASSED_PAWN_OFFSET S(-17, -9)
#define BISHOP_PAIR S(24, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(10, 11)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 1)
#define ROOK_SEMIOPEN S(10, 6)
#define KING_OPEN S(-43, -5)
#define KING_SEMIOPEN S(-9, 13)
#define KING_RING_ATTACKS S(14, -6)
#define PAWN_OPP_PER_MAJOR S(-7, -8)
