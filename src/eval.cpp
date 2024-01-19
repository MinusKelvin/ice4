// loss: 0.07902    train id: 2024-01-19-23-34-11-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(218, 233), S(230, 238), S(225, 232), S(225, 240),
    S(237, 301), S(234, 306), S(239, 302), S(234, 306),
    S(290, 556), S(302, 575), S(296, 549), S(320, 563),
    S(608, 1094), S(593, 1131), S(611, 1094), S(604, 1145),
};
int PAWN_SHIELD[] = {S(-10, -4), S(-4, -17), S(-2, -11), S(7, -6)};
int MOBILITY[] = {0, S(4, 8), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"8QM522. AOF=<:.'?HRNLA3*EQZYQE?2Sazb_^I<?*\\cEl/A86++041/$($#)6+3& ((;A+4759BKE!!6BEIUPCVPBG]x?YD9bD \"3H60*]vbh ).1*0>=5DGM<:MK\"$ $'30))64-&)*7.2;B ,43*502$'+- (,/-.22010+*)$# *:D:700$(7<;2/-)102246147537;<<BVBRUIPD~¡¡¨¬¤2.% &)5.5@5-'/;4TWLA7:CIrrbNGJWd¡\\Tm{¡uAACM_$44 5MJ6H[[L8@E3 Lgl]mvzf|q -**%)*,1144+353$%& )(('$$'%(+*(% #,*$)0/>FB8GOT"

#define PAWN_OFFSET S(25, 72)
#define PASSED_PAWN_OFFSET S(-16, -11)
#define BISHOP_PAIR S(24, 46)
#define DOUBLED_PAWN S(5, 14)
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(8, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 3)
#define ROOK_SEMIOPEN S(10, 10)
#define KING_OPEN S(-45, -3)
#define KING_SEMIOPEN S(-9, 16)
#define KING_RING_ATTACKS S(14, -6)
#define HAS_CASTLE_RIGHTS S(25, -18)
