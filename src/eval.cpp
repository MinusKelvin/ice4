// loss: 0.07904    train id: 2024-08-15-08-53-54-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(215, 236), S(226, 242), S(222, 235), S(222, 243),
    S(232, 305), S(229, 311), S(234, 306), S(229, 310),
    S(301, 539), S(314, 557), S(307, 532), S(330, 546),
    S(614, 1074), S(599, 1111), S(617, 1074), S(611, 1123),
};
int PAWN_SHIELD[] = {S(-2, -8), S(4, -22), S(5, -16), S(13, -9)};
int MOBILITY[] = {0, S(5, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"8PN521, ANF><:-(@HROLB2*FPZYRE>2U`zb`^H<=(]bDk1D:7--3643(*&%,7.8)\"**>C,686:CLF! 6BEIVQFUNBH]x@XA@^F #5Q:66h|i{ )02+2@>6GIP><OM\"$ $'30))85-')*7/28? ,32)4/1\"&)+ ),/-.33130+,*$# (8D96/0$&5<;2.-*//2335244337::=AR>QUGND}¤£©«¦1.% &)5.3?4-'/;4RVK?59CHqraLFIWc¡[Sn|£t>>BN]\"3/ 3KH4CWVF3;A, Kej\\ltyezo -**$(*+1144*353!#& '''%\"#&%(**(% \",*$)0.>GB8GPV"

#define PAWN_OFFSET S(24, 63)
#define PASSED_PAWN_OFFSET S(-15, -8)
#define BISHOP_PAIR S(26, 43)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(10, 11)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(10, 6)
#define KING_OPEN S(-44, -5)
#define KING_SEMIOPEN S(-10, 14)
#define KING_RING_ATTACKS S(14, -6)
#define PAWN_OPP_NO_MAJOR S(-32, 19)
