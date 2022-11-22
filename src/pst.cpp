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
    unpack(0, PAWN,   "bbbb(F?1.C>A(@BR+DM\\ /f`/EZ~bbbb", 1.718, -15);
    unpack(0, KNIGHT, "1@B9<;CD<IIJDRJLWOb[`mr~YSwd HIO", 2.721, 164);
    unpack(0, BISHOP, "I\\A;TTaCSNCJIEGYKLccoyp~TZdS%:/ ", 1.685, 253);
    unpack(0, ROOK,   ",01: 200,>*,,2\"4HKBEYkade[jmwx~t", 2.043, 281);
    unpack(0, QUEEN,  "WBCH_YVFOF30<=,#U58 ~q6I\\\"@1+FPN", 1.735, 846);
    unpack(0, KING,   "~{hgrdPB<B('/+' @<:8FNFLKLLPJKLK", 3.750, -156);
    unpack(1, PAWN,   "####$*&(#) %%)!#22'$V^HF~|sd####", 1.57, 95);
    unpack(1, KNIGHT, "?DPXVY`eWcir`ixzaqy~WcnnM\\]b EPZ", 1.475, 210);
    unpack(1, BISHOP, " 6*;:RCWHXlmBcquPkm~RTk\\.PLS?DFO", 0.604, 297);
    unpack(1, ROOK,   " <EJ&09=*0;D<DRVLS_bSace]n}~ditx", 0.443, 523);
    unpack(1, QUEEN,  " *+7\"+8D9JZXOZbrOfn}BQ~tB^jwPOai", 1.041, 949);
    unpack(1, KING,   "1CE9HT[_T^hoVgs{[s{~ax~~Wpso JPT", 1.162, -72);
}
