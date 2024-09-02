// loss: 0.0791    train id: 2024-09-02-19-08-48-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(216, 233), S(228, 238), S(224, 233), S(223, 240),
    S(236, 301), S(233, 306), S(238, 302), S(233, 305),
    S(291, 558), S(304, 576), S(298, 550), S(321, 564),
    S(608, 1093), S(594, 1130), S(611, 1093), S(605, 1142),
};
int PAWN_SHIELD[] = {S(-3, -8), S(4, -21), S(5, -16), S(13, -10)};
int MOBILITY[] = {0, S(5, 10), S(1, 0), S(3, 5), S(2, 3), S(1, 4), S(-3, 5)};

#define DATA_STRING L"6OM410+ @MD<:9,(>FPMK@1*DOYXPD=2T_xa^]G<<&\\\177`Ch,4' ('<A+4658AKD  5BDHTPGUNCG]wAW@?]D #4G50*\\t^i )02*2@>6FHO=;NM\"$ $'30))85-')*7/28? +32)5/1#&*, ),/..33131,,*$$ )4>42.0%)487//.*/,./15244//27:=@P:LOCLCzx¤¤ :>5* #*.XZL813>I\177uF?Xewa))-:I\"2/ 3JG4EXWI6=C0 Lfk]luzf|q -*)$(*+1144*353!#& '''&\"#&%'*)'% \",*#(/.>FA8GOT"

#define PAWN_OFFSET S(25, 72)
#define PASSED_PAWN_OFFSET S(-15, 13)
#define BISHOP_PAIR S(23, 46)
#define TEMPO S(11, 12)
#define ISOLATED_PAWN S(10, 9)
#define PROTECTED_PAWN S(7, 7)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 8)
#define KING_OPEN S(-44, -5)
#define KING_SEMIOPEN S(-10, 13)
#define KING_RING_ATTACKS S(14, -6)
