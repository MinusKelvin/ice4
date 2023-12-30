// loss: 0.07876    train id: 2023-12-31-09-30-54-frzn-big3

#define S(a, b) (a + (b * 0x10000))

#define PHASE_BASE S(132, 4)
int PHASE[] = {0, S(8, 1), S(-8, -1), S(-10, 0), S(-12, 6), S(-31, 17), 0};

int QUADRANTS[] = {
    S(147, 138), S(147, 159), S(145, 151), S(145, 170),
    S(175, 145), S(177, 145), S(176, 146), S(176, 150),
    S(314, 120), S(326, 126), S(310, 133), S(318, 158),
    S(659, 202), S(677, 181), S(657, 208), S(677, 206),
};
int DOUBLED_PAWN[] = {S(11, -2), S(9, -18), S(10, 1), S(7, 11), S(6, 13), S(11, 1), S(8, -16), S(17, -7)};
int PROTECTED_PAWN[] = {0, S(3, 6), S(3, 5)};
int PAWN_SHIELD[] = {S(-4, 6), S(-9, 17), S(-7, 19), S(-6, 29)};
int MOBILITY[] = {0, S(4, 2), S(0, 2), S(3, 3), S(1, 2), S(2, 1), S(3, -9)};

#define DATA_LOW " \"+0,+()!#,..*)(%))**+,++,+*,./119168694\\S_hidmp''\" !#*%)0(%#&,(;=60*,25LMB736>DidV@;HS]`hP**78<!,* -=<.9DD:48;0 ;AC<DIKBLQTFNWX &&&\"%%&()**%)**!!# %$$#\"!#!#$#\"\"#\"& \"$% +/,#.17VtlGCC;.bpaUPP<8\\`rkfV>9_izzkXK=qx=\"upRK8 c( l1]Sfbe[X\\_SQNMIJO\\CYD:HJ_`7KQNWch] 'N[dci[ZusxuwAZ,c`4h# RhYSZx-x-- $&\"#52,:;C2.@?(' %.:6,.?:1-(.@6:AL 4;;/B:;)/53\",136498?61.4*& "
#define DATA_HIGH "                                                                                                                                                                                                                  !!       !!                                                 ! !  !         ! !                                                                "

#define PAWN_OFFSET S(42, -23)
#define PASSED_PAWN_OFFSET S(-4, -52)
#define BISHOP_PAIR S(25, 13)
#define TEMPO S(6, 10)
#define ISOLATED_PAWN S(4, 7)
#define ROOK_OPEN S(2, 30)
#define ROOK_SEMIOPEN S(6, 9)
#define KING_OPEN S(-2, -52)
#define KING_SEMIOPEN S(8, -18)
