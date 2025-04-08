// loss: 0.07814    train id: 2025-04-08-10-35-33-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(21, 78), S(118, 278), S(150, 357), S(281, 655), S(554, 1233), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(40, 0), S(20, 0), S(27, 0), S(25, 0), S(25, 0), 0};

#define DATA_STRING L"        8SP854.!<I@974($=ENKI=0)COWYRD>0OVxge_J=( D\\R=;4        ~£¤¬¯¬«¥¬¬¶±±£¤µÁ°Ä¦­ºÅÑäÞÇ¦²Æ´Þª¡ 1`®?%Rptqqsjsuy~sy{ws\177y|}vsvyx{jv}\177uoe}|xiuohpysnN78): IK.18>C<:7!)0279;\"$*(+34@3! *.3/50$,29:CE:*99EWSdE3-CME\\HM=789:HIL=DLIKEH=DKPQQRSKILJIIOVQE??>FDJN<=;8<?CH;C<?CHKIB04* D=M!\"9H4;A+|evh}`hwr_RTaz]d\177U1+Xq6Y\177W- G]9O}O?`t}c>uisS\\d^z|@]Hu¥ ),2'$&%$ \" %14h'# \"',71[ XY_dfYN[]i¾         \"'17*,*.%'/0.()*+/'&#(//98+%#,4<MX:( /CE_dZ.-4:@        KWv|~x^Jq\177ytvx¨¦¦§¬²£\177}tp{\177f,| (9+55<2 3.28475/ABBAAA@<;EGD>@?2?C?GA@A:GBB9>BB>8CEKD?D4PSWUSYQB535/(/( 3173*&!,:;>;34,+FLGB:?87QNNF<:;;QMNB=;3>T]VOQFIANVWSPPMKK@;T>.);E>>EE4 6KX]P[d`TWkjxjrsq^hvne_yxax·pyv`szkwj\177{zYgwucq{{¯® °»½±§¸¾É¾¾¾Ã»ª±ÁÀ: =\"  )!&#'  2U|·ò TF3+,,,D A]isy~ &4T©Ǿ"

#define EG_OFFSET 422

#define BISHOP_PAIR S(23, 53)
#define TEMPO S(10, 14)
#define ISOLATED_PAWN S(7, 8)
#define PROTECTED_PAWN S(12, 16)
#define ROOK_OPEN S(23, 5)
#define ROOK_SEMIOPEN S(9, 12)
#define PAWN_SHIELD_INDEX 384
#define PAWN_SHIELD S(-6, -30)
#define KING_OPEN S(-44, -2)
#define KING_SEMIOPEN S(-10, 11)
#define MOBILITY_INDEX 387
#define MOBILITY S(-3, 0)
#define PASSER_RANK_INDEX 393
#define PASSER_RANK S(-14, -58)
#define OWN_KING_PASSER_DIST_INDEX 400
#define OWN_KING_PASSER_DIST S(-7, 0)
#define OPP_KING_PASSER_DIST_INDEX 408
#define OPP_KING_PASSER_DIST S(-59, -36)
#define PHALANX_RANK_INDEX 415
#define PHALANX_RANK S(-46, 5)
#define ATTACKER_NO_QUEEN S(-96, 0)
