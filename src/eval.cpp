// loss: 0.07881    train id: 2024-01-05-18-19-07-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int QUADRANTS[] = {
    S(217, 243), S(230, 247), S(224, 241), S(227, 247),
    S(238, 305), S(237, 312), S(240, 306), S(237, 312),
    S(295, 557), S(309, 567), S(302, 549), S(326, 557),
    S(611, 1095), S(594, 1124), S(614, 1094), S(608, 1133),
};
int PAWN_SHIELD[] = {S(-2, -10), S(3, -22), S(5, -17), S(12, -12)};
int MOBILITY[] = {0, S(4, 7), S(2, -3), S(3, 4), S(2, 3), S(1, 3), S(-3, 4)};
int THREATS[] = {0, S(-7, 16), S(7, 15), S(12, 20), S(11, 13), S(39, 46), S(0, 0)};

#define DATA_STRING L"8RO722. @PF>;:.&?FRLIA1*DQYVND?0Tby`]]H<?%]~~b@l0A86-,1530%'%$*7-6&\"*)=C-687;CLG !7CFJUQCZOEH\\|>U=<ZD \"3L71._vak (/2+2?>6GIP>ASQ#% %*42+):70((-:.27? -43*713&*-/ ),///55472.0.'' *8A86/0$'6;;2.-)20454715877:=<>CXEUXLRE¢§¢­³§0-$ $'4,2?3,%-91RUK>48AGpp`LEHUa¡[Slz s@DBH_\"30 /GE1>RQA/8>) Njn^p|hq --,$)+-1256+874%&)\"'&&$! #\"%&%#' $0) %-)3@;1>GJ"

#define PAWN_OFFSET S(25, 75)
#define PASSED_PAWN_OFFSET S(-17, -8)
#define BISHOP_PAIR S(23, 43)
#define DOUBLED_PAWN S(5, 15)
#define TEMPO S(13, 15)
#define ISOLATED_PAWN S(8, 8)
#define PROTECTED_PAWN S(7, 8)
#define ROOK_OPEN S(22, 2)
#define ROOK_SEMIOPEN S(14, -1)
#define KING_OPEN S(-43, -2)
#define KING_SEMIOPEN S(-9, 15)
#define KING_RING_ATTACKS S(14, -6)
