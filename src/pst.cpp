int16_t PST[2][25][SQUARE_SPAN];

void unpack(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = white_section[rank+7-file] = v;
            black_section[70-rank+file] = black_section[77-rank-file] = -v;
        }
    }
}

void init_pst() {
    unpack(0, PAWN, "VVVV!G=++E=A <@Y'FPl.Fz~V`ckVVVV", 1.092, 41);
    unpack(0, KNIGHT, "?IRQQR[_R`ek\\hnpel{{apx~X`ni QT]", 1.585, 187);
    unpack(0, BISHOP, "#6$'4>>79=@D1>DM7DLUGFML*<=9 ,+-", 1.0, 290);
    unpack(0, ROOK, "\".18 +-/'0,1/44:>ACEEMMOKPYXPPVV", 1.0, 459);
    unpack(0, QUEEN, "( \".+.8529:56@<DDCMLPRXY<6NR,9KQ", 1.0, 898);
    unpack(0, KING, "~}\\ZmV8 87((?=;9EDDCGIGHGHHIGGGG", 2.16, -85);
    unpack(1, PAWN, "1111\",'(!* &#*\"#13)$U_KD~~vi1111", 1.346, 77);
    unpack(1, KNIGHT, "?IRQQR[_R`ek\\hnpel{{apx~X`ni QT]", 1.585, 187);
    unpack(1, BISHOP, "#6$'4>>79=@D1>DM7DLUGFML*<=9 ,+-", 1.0, 290);
    unpack(1, ROOK, "\".18 +-/'0,1/44:>ACEEMMOKPYXPPVV", 1.0, 459);
    unpack(1, QUEEN, "( \".+.8529:56@<DDCMLPRXY<6NR,9KQ", 1.0, 898);
    unpack(1, KING, " 34(7EKO:IOW:MZcD^hjKgmmC]`]);>A", 1.0, -44);
}
