struct Move {
    uint8_t from;
    uint8_t to : 7;
    uint8_t promo : 1;

    void put_with_newline() {
        putchar(from%10+96);
        putchar(from/10+47);
        putchar(to%10+96);
        putchar(to/10+47);
        putchar(promo ? 'q' : ' ');
        putchar('\n');
    }
};

Move create_move(uint8_t from, uint8_t to, uint8_t promo) {
    // removing Move here increases size, somehow
    return Move{from, to, promo};
}

struct TtData {
    uint16_t key;
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;
};

// 8MB. Replaced for TCEC builds by the minifier.
#define HASH_SIZE 1048576
auto TT = new atomic<TtData>[HASH_SIZE]();

#ifdef OPENBENCH
int TT_SIZE = HASH_SIZE;
#else
#define TT_SIZE HASH_SIZE
#endif

struct Board {
    uint8_t board[120];
    uint8_t castle_rights;
    uint8_t king_sq[2];
    uint8_t piece_counts[25];
    uint8_t piece_file_counts[25][10];
    uint8_t ep_square;
    uint8_t stm;
    uint8_t phase;
    uint8_t pawn_eval_dirty;
    uint8_t check;
    int32_t inc_eval;
    int32_t pawn_eval;
    uint64_t zobrist;
    uint64_t pawn_hash;
    uint64_t material_hash;

    void edit(int square, int piece) {
        if ((board[square] & 7) == PAWN || (piece & 7) == PAWN || (piece & 7) == KING) {
            pawn_eval_dirty = 1;
        }
        zobrist ^= ZOBRIST[board[square]][square];
        piece_file_counts[board[square]][square % 10]--;
        if (board[square] & 7) {
            material_hash ^= ZOBRIST[board[square]][piece_counts[board[square]]--];
        }
        if ((board[square] & 7) == PAWN) {
            pawn_hash ^= ZOBRIST[board[square]][square];
        } else {
            inc_eval -= PST[board[square]][square-A1];
        }
        phase -= PHASE[board[square] & 7];
        board[square] = piece;
        zobrist ^= ZOBRIST[board[square]][square];
        piece_file_counts[board[square]][square % 10]++;
        if (board[square] & 7) {
            material_hash ^= ZOBRIST[board[square]][++piece_counts[board[square]]];
        }
        if ((board[square] & 7) == PAWN) {
            pawn_hash ^= ZOBRIST[board[square]][square];
        } else {
            inc_eval += PST[board[square]][square-A1];
        }
        phase += PHASE[board[square] & 7];
        if ((board[square] & 7) == KING) {
            king_sq[!(board[square] & WHITE)] = square;
        }
    }

    Board() {
        memset(this, 0, sizeof(Board));
        memset(board, INVALID, 120);
        stm = WHITE;
        for (int i = 0; i < 8; i++) {
            edit(i + A1, LAYOUT[i] | WHITE);
            edit(i + A8, LAYOUT[i] | BLACK);
            edit(i + A2, WHITE_PAWN);
            edit(i + A7, BLACK_PAWN);
            edit(i + A3, EMPTY);
            edit(i + A4, EMPTY);
            edit(i + A5, EMPTY);
            edit(i + A6, EMPTY);
        }
    }

    int attacked(int ksq, int by) {
        if (
            board[ksq + (by & WHITE ? -10 : 10) + 1] == (PAWN | by) ||
            board[ksq + (by & WHITE ? -10 : 10) - 1] == (PAWN | by)
        ) {
            return 1;
        }
        for (int i = 0; i < 16; i++) {
            int sq = ksq + RAYS[i];
            if (i < 8 && board[sq] == (KING | by)) {
                return 1;
            }
            while (i < 8 && !board[sq]) {
                sq += RAYS[i];
            }
            if (i < 8 && board[sq] == (QUEEN | by) || board[sq] == (SLIDER[i] | by)) {
                return 1;
            }
        }
        return 0;
    }

    int make_move(Move mv, int promo = QUEEN) {
        int piece = mv.promo ? promo | stm : board[mv.from];
        #define NSTM (stm ^ INVALID)
        edit(mv.to, piece);
        edit(mv.from, EMPTY);

        // handle en-passant
        zobrist ^= ZOBRIST[EMPTY][ep_square];
        int ep = stm != WHITE ? 10 : -10;
        if ((piece & 7) == PAWN) {
            if (mv.to == ep_square) {
                edit(mv.to + ep, EMPTY);
            }
            if (mv.to + ep == mv.from - ep) {
                ep_square = mv.to + ep;
            } else {
                ep_square = 0;
            }
        } else {
            ep_square = 0;
        }
        zobrist ^= ZOBRIST[EMPTY][ep_square];

        // handle castling
        zobrist ^= ZOBRIST[EMPTY][castle_rights];
        int back_rank = stm != WHITE ? A8 : A1;
        if ((piece & 7) == KING && mv.from == back_rank + 4) {
            if (mv.to == back_rank + 6) {
                edit(back_rank + 7, EMPTY);
                edit(back_rank + 5, stm | ROOK);
                if (attacked(back_rank + 4, NSTM) || attacked(back_rank + 5, NSTM)) {
                    return 1;
                }
            }
            if (mv.to == back_rank + 2) {
                edit(back_rank + 0, EMPTY);
                edit(back_rank + 3, stm | ROOK);
                if (attacked(back_rank + 4, NSTM) || attacked(back_rank + 3, NSTM)) {
                    return 1;
                }
            }
            castle_rights |= 3 << 2*(stm != WHITE);
        }

        if (mv.from == A1 || mv.to == A1) {
            castle_rights |= WHITE_LONG_CASTLE;
        }
        if (mv.from == H1 || mv.to == H1) {
            castle_rights |= WHITE_SHORT_CASTLE;
        }
        if (mv.from == A8 || mv.to == A8) {
            castle_rights |= BLACK_LONG_CASTLE;
        }
        if (mv.from == H8 || mv.to == H8) {
            castle_rights |= BLACK_SHORT_CASTLE;
        }
        zobrist ^= ZOBRIST[EMPTY][castle_rights];

        if (attacked(king_sq[stm != WHITE], NSTM)) {
            return 1;
        }

        check = attacked(king_sq[stm == WHITE], stm);
        stm ^= INVALID;
        zobrist ^= ZOBRIST[EMPTY][0];
        return 0;

        #undef NSTM
    }

    void movegen(Move list[], int& count, int quiets, int& mobility) {
        // King ring attacks: 30 bytes (v5)
        // 8.0+0.08: 7.47 +- 4.89 [382, 1239, 1891, 1190, 299] 0.25 elo/byte
        // Mobility: 26 bytes (v5)
        // 8.0+0.08: 103.92 +- 5.26 [970, 1765, 1563, 604, 98] 4.00 elo/byte
        int king_ring[120] = {};
        #define OTHER (stm ^ INVALID)
        count = 0;
        mobility = 0;
        for (int i = 0; i < 8; i++) {
            king_ring[king_sq[stm == WHITE] + RAYS[i]] = KING_RING_ATTACKS;
        }
        for (int sq = A1; sq <= H8; sq++) {
            // skip empty squares & opponent squares (& border squares)
            if ((board[sq] & INVALID) != stm) {
                continue;
            }

            int piece = board[sq] & 7;

            if (piece == KING && sq == (stm == WHITE ? E1 : E8) && quiets) {
                if (!(castle_rights >> 2*(stm != WHITE) & SHORT_CASTLE) &&
                        !board[sq+1] && !board[sq+2]) {
                    list[count++] = create_move(sq, sq + 2, 0);
                }
                if (!(castle_rights >> 2*(stm != WHITE) & LONG_CASTLE) &&
                        !board[sq-1] && !board[sq-2] && !board[sq-3]) {
                    list[count++] = create_move(sq, sq - 2, 0);
                }
            }

            if (piece == PAWN) {
                int dir = stm == WHITE ? 10 : -10;
                int promo = board[sq + dir + dir] == INVALID;
                if (!board[sq + dir]) {
                    mobility += MOBILITY[piece] + king_ring[sq + dir];
                    if (quiets || promo || board[sq + dir + dir + dir] == INVALID) {
                        list[count++] = create_move(sq, sq + dir, promo);
                    }
                    if (board[sq - dir - dir] == INVALID && !board[sq + dir + dir]) {
                        mobility += MOBILITY[piece] + king_ring[sq + dir+dir];
                        if (quiets) {
                            list[count++] = create_move(sq, sq + dir+dir, promo);
                        }
                    }
                }
                if (ep_square == sq + dir-1 || board[sq + dir-1] & OTHER && ~board[sq + dir-1] & stm) {
                    mobility += MOBILITY[piece] + king_ring[sq + dir-1];
                    list[count++] = create_move(sq, sq + dir-1, promo);
                }
                if (ep_square == sq + dir+1 || board[sq + dir+1] & OTHER && ~board[sq + dir+1] & stm) {
                    mobility += MOBILITY[piece] + king_ring[sq + dir+1];
                    list[count++] = create_move(sq, sq + dir+1, promo);
                }
            } else {
                for (int i = STARTS[piece]; i < ENDS[piece]; i++) {
                    int raysq = sq;
                    for (int j = 0; j < LIMITS[piece]; j++) {
                        raysq += RAYS[i];
                        if (board[raysq] & stm) {
                            break;
                        }
                        mobility += MOBILITY[piece] + king_ring[raysq];
                        if (board[raysq] & OTHER) {
                            list[count++] = create_move(sq, raysq, 0);
                            break;
                        } else if (quiets) {
                            list[count++] = create_move(sq, raysq, 0);
                        }
                    }
                }
            }
        }
        #undef OTHER
    }

    void calculate_pawn_eval(int ci, int color, int pawndir, int first_rank) {
        int shield_pawns = 0;
        int own_pawn = PAWN | color;
        int opp_pawn = own_pawn ^ INVALID;
        // King on (semi-)open file: 23 bytes (v5)
        // 8.0+0.08: 11.02 +- 4.70 [319, 1331, 1950, 1148, 252] 0.48 elo/byte
        if (!piece_file_counts[own_pawn][king_sq[ci] % 10]) {
            pawn_eval += piece_file_counts[opp_pawn][king_sq[ci] % 10] ? KING_SEMIOPEN : KING_OPEN;
        }
        for (int file = 1; file < 9; file++) {
            // Isolated pawns: 17 bytes (v5)
            // 8.0+0.08: 11.88 +- 4.85 [381, 1311, 1835, 1217, 257] 0.70 elo/byte
            if (!piece_file_counts[own_pawn][file-1] && !piece_file_counts[own_pawn][file+1]) {
                pawn_eval -= ISOLATED_PAWN * piece_file_counts[own_pawn][file];
            }
            for (int rank = 6; rank > 0; rank--) {
                int sq = file + first_rank + rank * pawndir;
                if (board[sq] == own_pawn) {
                    pawn_eval += get_data(PASSER_RANK_INDEX + rank)
                        + get_data(KING_PASSER_DIST_INDEX + max(
                            abs(sq / 10 - king_sq[ci] / 10),
                            abs(file - king_sq[ci] % 10)
                        ))
                        + get_data(KING_PASSER_DIST_INDEX + 8 + max(
                            abs(sq / 10 - king_sq[!ci] / 10),
                            abs(file - king_sq[!ci] % 10)
                        ))
                        + (PASSER_RANK + KING_PASSER_DIST);
                    break;
                }
                if (board[sq] == opp_pawn || board[sq-1] == opp_pawn || board[sq+1] == opp_pawn) {
                    break;
                }
            }
            for (int rank = 6; rank > 0; rank--) {
                int sq = file + first_rank + rank * pawndir;
                if (board[sq] == own_pawn) {
                    // Protected pawn: 32 bytes (v5)
                    // 8.0+0.08: 9.70 +- 4.84 [353, 1319, 1869, 1172, 287] 0.30 elo/byte
                    if (board[sq - pawndir+1] == own_pawn || board[sq - pawndir-1] == own_pawn) {
                        pawn_eval += PROTECTED_PAWN;
                    }
                    if (board[sq - 1] == own_pawn) {
                        pawn_eval += get_data(PHALANX_RANK_INDEX + rank) + PHALANX_RANK;
                    }
                    if (king_sq[ci] % 10 > 4) {
                        sq += 9 - file - file;
                    }
                    pawn_eval += PST[own_pawn][sq-A1];
                }
            }
        }
        // Pawn shield: 66 bytes (v5)
        // 8.0+0.08: 12.76 +- 4.75 [337, 1368, 1862, 1191, 242] 0.19 elo/byte
        for (int dx = -1; dx < 2; dx++) {
            shield_pawns += board[king_sq[ci]+dx+pawndir] == own_pawn
                || board[king_sq[ci]+dx+pawndir*2] == own_pawn;
        }
        if (king_sq[ci] / 10 == first_rank / 10) {
            pawn_eval += get_data(PAWN_SHIELD_INDEX + shield_pawns) + PAWN_SHIELD;
        }
    }

    int eval(int stm_eval) {
        if (pawn_eval_dirty) {
            pawn_eval = 0;
            calculate_pawn_eval(1, BLACK, -10, 90);
            pawn_eval = -pawn_eval;
            calculate_pawn_eval(0, WHITE, 10, 20);
            pawn_eval_dirty = 0;
        }

        // Bishop pair: 29 bytes (v5)
        // 8.0+0.08: 25.79 +- 4.96 [466, 1393, 1806, 1086, 249] 0.89 elo/byte
        int e = inc_eval + pawn_eval + BISHOP_PAIR * ((piece_counts[WHITE_BISHOP] >= 2) - (piece_counts[BLACK_BISHOP] >= 2));
        // Rook on (semi-)open file: 42 bytes (v5)
        // 8.0+0.08: 9.83 +- 4.84 [344, 1347, 1852, 1166, 293] 0.23 elo/byte
        for (int file = 1; file < 9; file++) {
            if (!piece_file_counts[WHITE_PAWN][file]) {
                e += (piece_file_counts[BLACK_PAWN][file] ? ROOK_SEMIOPEN : ROOK_OPEN)
                    * piece_file_counts[WHITE_ROOK][file];
            }
            if (!piece_file_counts[BLACK_PAWN][file]) {
                e -= (piece_file_counts[WHITE_PAWN][file] ? ROOK_SEMIOPEN : ROOK_OPEN)
                    * piece_file_counts[BLACK_ROOK][file];
            }
        }
        stm_eval += stm == WHITE ? e : -e;
        return ((int16_t)stm_eval * phase + ((stm_eval + 0x8000) >> 16) * (24 - phase)) / 24;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;

#ifdef OPENBENCH
void parse_fen() {
    int rank = 7;
    int file = 0;
    char *word = strtok(0, " \n");
    for (char c = *word++; c; c = *word++) {
        int sq = (rank * 10) + file + A1;
        file++;
        switch (c) {
            case 'P':
                ROOT.edit(sq, WHITE | PAWN);
                break;
            case 'N':
                ROOT.edit(sq, WHITE | KNIGHT);
                break;
            case 'B':
                ROOT.edit(sq, WHITE | BISHOP);
                break;
            case 'R':
                ROOT.edit(sq, WHITE | ROOK);
                break;
            case 'Q':
                ROOT.edit(sq, WHITE | QUEEN);
                break;
            case 'K':
                ROOT.edit(sq, WHITE | KING);
                break;
            case 'p':
                ROOT.edit(sq, BLACK | PAWN);
                break;
            case 'n':
                ROOT.edit(sq, BLACK | KNIGHT);
                break;
            case 'b':
                ROOT.edit(sq, BLACK | BISHOP);
                break;
            case 'r':
                ROOT.edit(sq, BLACK | ROOK);
                break;
            case 'q':
                ROOT.edit(sq, BLACK | QUEEN);
                break;
            case 'k':
                ROOT.edit(sq, BLACK | KING);
                break;
            case '/':
                file = 0;
                rank--;
                break;
            default:
                file += c - '1';
                for (int i = 0; i < c - '0'; i++) {
                    ROOT.edit(sq+i, 0);
                }
                break;
        }
    }

    if (*strtok(0, " \n") == 'b') {
        ROOT.stm = BLACK;
        ROOT.zobrist ^= ZOBRIST[EMPTY][0];
    }

    word = strtok(0, " \n");
    int remove_white_short = 1;
    int remove_white_long = 1;
    int remove_black_short = 1;
    int remove_black_long = 1;
    for (char c = *word++; c; c = *word++) {
        switch (c) {
            case 'K':
                remove_white_short = 0;
                break;
            case 'Q':
                remove_white_long = 0;
                break;
            case 'k':
                remove_black_short = 0;
                break;
            case 'q':
                remove_black_long = 0;
                break;
        }
    }
    ROOT.zobrist ^= ZOBRIST[EMPTY][ROOT.castle_rights];
    if (remove_white_short) {
        ROOT.castle_rights |= WHITE_SHORT_CASTLE;
    }
    if (remove_white_long) {
        ROOT.castle_rights |= WHITE_LONG_CASTLE;
    }
    if (remove_black_short) {
        ROOT.castle_rights |= BLACK_SHORT_CASTLE;
    }
    if (remove_black_long) {
        ROOT.castle_rights |= BLACK_LONG_CASTLE;
    }
    ROOT.zobrist ^= ZOBRIST[EMPTY][ROOT.castle_rights];

    word = strtok(0, " \n");
    if (*word != '-') {
        ROOT.ep_square = (word[1] - '1') * 10 + word[0] - 'a' + A1;
    }

    strtok(0, " \n"); // halfmove clock

    strtok(0, " \n"); // fullmove counter
}
#endif
