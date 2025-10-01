// loss: 0.0507    train id: 2025-10-02-08-39-15-big3

#define S(a, b) (a + (b * 0x10000))

int MATERIAL[] = {0, S(4, 92), S(122, 429), S(169, 457), S(279, 843), S(579, 1581), S(0, 0)};
int KING_ATTACK_WEIGHT[] = {0, S(49, 0), S(19, 0), S(33, 0), S(30, 0), S(29, 0), 0};

#define DATA_STRING L"TwkXHIA1XfTUOJ8.W^fi`QD4cjw{hZQ=u¬]O FczmOL4`r±  7AMPQD:Zfhb^dY  .-$*)6*2*) *85#$ (083@3ag^WMRH !\" #!'1/ptJ& UacWfK =2gc /4?+',('  \"$4*k/$ )05H5_ ^]ego\\īĺńŔŻ +5@I39<@15>;928<;A4//4>BNK6+-9GVswF# :ghlq7.BPc%<XgjS; +<PUQI9  %88:5-8+-*402*   $499LK7<?5-2'  \"J]jgrp ->DIB6/ .CVisl%%EZj\\\\C I)& , '#&  6dóĽ hV=1318H Eiv /<fāɲ"

#define EG_OFFSET 166

#define BISHOP_PAIR S(22, 80)
#define TEMPO S(21, 33)
#define ISOLATED_PAWN S(7, 9)
#define PROTECTED_PAWN S(17, 20)
#define ROOK_OPEN S(33, 11)
#define ROOK_SEMIOPEN S(12, 13)
#define PAWN_SHIELD_INDEX 128
#define PAWN_SHIELD S(-17, -32)
#define KING_OPEN S(-66, 0)
#define KING_SEMIOPEN S(-13, 12)
#define MOBILITY_INDEX 131
#define MOBILITY S(-6, 1)
#define PASSER_RANK_INDEX 137
#define PASSER_RANK S(-16, -80)
#define OWN_KING_PASSER_DIST_INDEX 144
#define OWN_KING_PASSER_DIST S(-15, 0)
#define OPP_KING_PASSER_DIST_INDEX 152
#define OPP_KING_PASSER_DIST S(-63, -40)
#define PHALANX_RANK_INDEX 159
#define PHALANX_RANK S(-266, -1)
#define ATTACKER_NO_QUEEN S(-92, 0)
