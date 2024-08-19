// loss: 0.07902    train id: 2024-08-19-10-49-06-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 234), S(230, 238), S(224, 233), S(225, 240),
    S(236, 301), S(235, 306), S(238, 303), S(237, 306),
    S(293, 558), S(305, 576), S(299, 551), S(323, 565),
    S(609, 1096), S(596, 1132), S(612, 1096), S(609, 1144),
};
int PAWN_SHIELD[] = {S(-3, -7), S(3, -21), S(5, -16), S(13, -9)};
int MOBILITY[] = {0, S(4, 9), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};
int PAWN_THREAT[] = {0, S(0, 0), S(30, 13), S(35, 31), S(47, 8), S(31, 2), S(0, 0)};

#define DATA_STRING L"8PM521, @LD=;9,&?FQMK@1)ENXVPD=1U_w`^\\G<; \\zz]7i3D;8/.4842&*'&,8-5&\"*)=D,577:CMG  5CEIUQGVNGK^\177AW@?]D #3H71+^wak )02+2@>5FHP>=PN\"$ $(31*)75.(*,9/27? ,32)501#'+- ),/./43121,-+&% (7C85.0$&5;;1--*/.1234144236::<AR>QTGND~£¢¨­¦1.% &*6.4A6.(0<5TXMA7;DIrtcNGKXd¢\\Uo~ £w@@CM^\"3/ 3JG4EXXI6=C0 Lfk]muzg|q -**$(*+1144*363!#& '''&\"#&$'))'% \",*#(/.>FA7FNT"

#define PAWN_OFFSET S(25, 68)
#define PASSED_PAWN_OFFSET S(-17, -8)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(12, 12)
#define ISOLATED_PAWN S(10, 11)
#define PROTECTED_PAWN S(8, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -4)
#define KING_SEMIOPEN S(-10, 13)
#define KING_RING_ATTACKS S(14, -6)
