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
    unpack(0, PAWN, "]]]](D?3-B>A&<AP)CL\\ 1hc7M^~]]]]", 1.711, -5);
    unpack(0, KNIGHT, "AMUTTU]aUbfl^joqgn{{cqx~Zaoj MU^", 1.736, 186);
    unpack(0, BISHOP, "#7%'5??7:>BE2?EO8ENVHGOM*<=; ,+-", 1.0, 302);
    unpack(0, ROOK, "\".18 ,.0'0-1054;?BDEEMMOKPYYQQWX", 1.0, 480);
    unpack(0, QUEEN, ") \".+-8428955@;CDCMLPRXZ;4LQ+8KQ", 1.0, 954);
    unpack(0, KING, "~}hgsdO>7?$#0+( @<;9FMFKIJJNIIJI", 3.491, -140);
    unpack(1, PAWN, "%%%%$+&'#* %%*!#23(%W_IF~|se%%%%", 1.517, 93);
    unpack(1, KNIGHT, "AMUTTU]aUbfl^joqgn{{cqx~Zaoj MU^", 1.736, 186);
    unpack(1, BISHOP, "#7%'5??7:>BE2?EO8ENVHGOM*<=; ,+-", 1.0, 302);
    unpack(1, ROOK, "\".18 ,.0'0-1054;?BDEEMMOKPYYQQWX", 1.0, 480);
    unpack(1, QUEEN, ") \".+-8428955@;CDCMLPRXZ;4LQ+8KQ", 1.0, 954);
    unpack(1, KING, "1CC8FTZ_S]fmSeqyZqz}`x~~Vpso IOS", 1.129, -69);
}
