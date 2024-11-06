// loss: 0.07869    train id: 2024-11-06-16-21-18-frzn-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(-10, 30), S(140, 256), S(189, 293), S(300, 537), S(668, 973), S(0, 0)};
int MOBILITY[] = {0, S(4, 8), S(1, -1), S(4, 5), S(2, 3), S(1, 6), S(-3, 0)};

#define DATA_STRING L"Ztq[YUOC]j_ZXTHE_folj^PJeqyzse_Rs|o`' BRJ;<5jv¢  -3:<=5,LWYVRZL  )(#'%.&,!# +:99 #*/500*295-(0&  !\"$$%)&]Z< #N\\gEJ9 1-QN *,3# %02XZSSX^jdX ^\\`ccW .0=LFLSXLOMPIKRRQLLLMRJIFKQOYYNHGNTZgpZLFO_`CFA\" $%) 4HRYG<9#5FJHB6  !..1./;%((-)-(   %/209:,--)\"&# $ ;O]_i\\ (07795:$.:CMTK %8AIBB5 9! ! /Llÿ<f\\NGHHG< ;Q\\ekn #0Jî"

#define EG_OFFSET 160

#define BISHOP_PAIR S(23, 46)
#define TEMPO S(10, 12)
#define ISOLATED_PAWN S(8, 7)
#define PROTECTED_PAWN S(12, 13)
#define ROOK_OPEN S(24, 2)
#define ROOK_SEMIOPEN S(9, 11)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-10, -17)
#define KING_OPEN S(-44, -1)
#define KING_SEMIOPEN S(-10, 11)
#define KING_RING_ATTACKS S(13, -5)
#define PASSER_RANK_INDEX 131
#define PASSER_RANK S(-16, -51)
#define KING_PASSER_DIST_INDEX 138
#define KING_PASSER_DIST S(-112, -56)
#define PHALANX_RANK_INDEX 153
#define PHALANX_RANK S(0, 5)
