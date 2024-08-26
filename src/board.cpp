struct Move {
    uint8_t from;
    uint8_t to : 7;
    uint8_t promo : 1;

    Move() = default;
    Move(int f, int t=0, int p=0) : from(f), to(t), promo(p) {}

    void put_with_newline() {
        putchar(from%10+96);
        putchar(from/10+47);
        putchar(to%10+96);
        putchar(to/10+47);
        putchar(promo ? 'q' : ' ');
        putchar('\n');
    }
};

struct TtData {
    uint16_t key;
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;
};

// 8MB. Replaced for TCEC builds by the minifier.
#define HASH_SIZE 1048576
vector< atomic<TtData> > TT(HASH_SIZE);

#ifdef OPENBENCH
#define TT_SIZE_EXPR TT.size()
#else
#define TT_SIZE_EXPR HASH_SIZE
#endif

struct Board {
    uint8_t board[120];
    uint8_t castle_rights;
    uint8_t bishops[2];
    uint8_t king_sq[2];
    uint8_t piece_file_counts[23][10];
    uint8_t ep_square;
    uint8_t stm;
    uint8_t phase;
    uint8_t pawn_eval_dirty;
    uint8_t check;
    int32_t inc_eval;
    int32_t pawn_eval;
    uint64_t zobrist;
    uint64_t pawn_hash;

    void edit(int square, int piece) {
        if ((board[square] & 7) == PAWN || (piece & 7) == PAWN || (piece & 7) == KING) {
            pawn_eval_dirty = 1;
        }
        zobrist ^= ZOBRIST.pieces[board[square]][square];
        piece_file_counts[board[square]][square % 10]--;
        if ((board[square] & 7) == PAWN) {
            pawn_hash ^= ZOBRIST.pieces[board[square]][square];
        } else {
            inc_eval -= PST[board[square]][square-A1];
        }
        phase -= PHASE[board[square] & 7];
        if ((board[square] & 7) == BISHOP) {
            bishops[!(board[square] & WHITE)]--;
        }
        board[square] = piece;
        zobrist ^= ZOBRIST.pieces[board[square]][square];
        piece_file_counts[board[square]][square % 10]++;
        if ((board[square] & 7) == PAWN) {
            pawn_hash ^= ZOBRIST.pieces[board[square]][square];
        } else {
            inc_eval += PST[board[square]][square-A1];
        }
        phase += PHASE[board[square] & 7];
        if ((board[square] & 7) == BISHOP) {
            bishops[!(board[square] & WHITE)]++;
        }
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
        zobrist ^= ZOBRIST.ep[ep_square];
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
        zobrist ^= ZOBRIST.ep[ep_square];

        // handle castling
        zobrist ^= ZOBRIST.castle_rights[castle_rights];
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
        zobrist ^= ZOBRIST.castle_rights[castle_rights];

        if (attacked(king_sq[stm != WHITE], NSTM)) {
            return 1;
        }

        check = attacked(king_sq[stm == WHITE], stm);
        stm ^= INVALID;
        zobrist ^= ZOBRIST.stm;
        return 0;

        #undef NSTM
    }

    void movegen(Move list[], int& count, int quiets, int& mobility) {
        #define OTHER (stm ^ INVALID)
        count = 0;
        mobility = 0;
        for (int sq = A1; sq <= H8; sq++) {
            // skip empty squares & opponent squares (& border squares)
            if ((board[sq] & INVALID) != stm) {
                continue;
            }

            int piece = board[sq] & 7;

            if (piece == KING && sq == (stm == WHITE ? E1 : E8) && quiets) {
                if (!(castle_rights >> 2*(stm != WHITE) & SHORT_CASTLE) &&
                        !board[sq+1] && !board[sq+2]) {
                    list[count++] = Move(sq, sq + 2, 0);
                }
                if (!(castle_rights >> 2*(stm != WHITE) & LONG_CASTLE) &&
                        !board[sq-1] && !board[sq-2] && !board[sq-3]) {
                    list[count++] = Move(sq, sq - 2, 0);
                }
            }

            if (piece == PAWN) {
                int dir = stm == WHITE ? 10 : -10;
                int promo = board[sq + dir + dir] == INVALID ? QUEEN : 0;
                if (!board[sq + dir]) {
                    mobility += MOBILITY[piece];
                    if (quiets || promo || board[sq + dir + dir + dir] == INVALID) {
                        list[count++] = Move(sq, sq + dir, promo);
                    }
                    if (board[sq - dir - dir] == INVALID && !board[sq + dir + dir]) {
                        mobility += MOBILITY[piece];
                        if (quiets) {
                            list[count++] = Move(sq, sq + dir+dir, promo);
                        }
                    }
                }
                if (ep_square == sq + dir-1 || board[sq + dir-1] & OTHER && ~board[sq + dir-1] & stm) {
                    mobility += MOBILITY[piece];
                    list[count++] = Move(sq, sq + dir-1, promo);
                }
                if (ep_square == sq + dir+1 || board[sq + dir+1] & OTHER && ~board[sq + dir+1] & stm) {
                    mobility += MOBILITY[piece];
                    list[count++] = Move(sq, sq + dir+1, promo);
                }
            } else {
                for (int i = STARTS[piece]; i < ENDS[piece]; i++) {
                    int raysq = sq;
                    for (int j = 0; j < LIMITS[piece]; j++) {
                        raysq += RAYS[i];
                        if (board[raysq] & stm) {
                            break;
                        }
                        mobility += MOBILITY[piece];
                        if (board[raysq] & OTHER) {
                            list[count++] = Move(sq, raysq);
                            break;
                        } else if (quiets) {
                            list[count++] = Move(sq, raysq);
                        }
                    }
                }
            }
        }
        #undef OTHER
    }

    void calculate_pawn_eval(int ci, int color, int pawndir, int first_rank, int seventh_rank) {
        int shield_pawns = 0;
        int own_pawn = PAWN | color;
        int opp_pawn = own_pawn ^ INVALID;
        if (!piece_file_counts[own_pawn][king_sq[ci] % 10]) {
            pawn_eval += piece_file_counts[opp_pawn][king_sq[ci] % 10] ? KING_SEMIOPEN : KING_OPEN;
        }
        for (int file = 1; file < 9; file++) {
            // Isolated pawns: 18 bytes (b4d32e5 vs 7f7c2b5)
            // 8.0+0.08: 14.64 +- 5.20 (3128 - 2707 - 4165) 0.81 elo/byte
            // 60.0+0.6: 16.79 +- 4.82 (2749 - 2266 - 4985) 0.93 elo/byte
            if (!piece_file_counts[own_pawn][file-1] && !piece_file_counts[own_pawn][file+1]) {
                pawn_eval -= ISOLATED_PAWN * piece_file_counts[own_pawn][file];
            }
            for (int rank = seventh_rank; rank != first_rank; rank -= pawndir) {
                int sq = file+rank;
                if (board[sq] == own_pawn) {
                    if (king_sq[ci] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    pawn_eval += PST[own_pawn+6][sq-A1];
                }
                if (board[file+rank] == opp_pawn || board[file+rank-1] == opp_pawn || board[file+rank+1] == opp_pawn) {
                    break;
                }
            }
            for (int rank = seventh_rank; rank != first_rank; rank -= pawndir) {
                int sq = rank+file;
                if (board[sq] == own_pawn) {
                    if (board[sq - pawndir+1] == own_pawn || board[sq - pawndir-1] == own_pawn) {
                        pawn_eval += PROTECTED_PAWN;
                    }
                    if (king_sq[ci] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    pawn_eval += PST[own_pawn][sq-A1];
                }
            }
        }
        // Pawn shield: 65 bytes (f3241b8 vs 7f7c2b5)
        // 8.0+0.08: 19.58 +- 5.17 (3159 - 2596 - 4245) 0.30 elo/byte
        // 60.0+0.6: 14.88 +- 4.63 (2526 - 2098 - 5376) 0.23 elo/byte
        for (int dx = -1; dx < 2; dx++) {
            shield_pawns += board[king_sq[ci]+dx+pawndir] == own_pawn
                || board[king_sq[ci]+dx+pawndir*2] == own_pawn;
        }
        pawn_eval += (king_sq[ci] / 10 == first_rank / 10) * PAWN_SHIELD[shield_pawns];
    }

    int eval(int stm_eval) {
        if (pawn_eval_dirty) {
            pawn_eval = 0;
            calculate_pawn_eval(1, BLACK, -10, 90, 30);
            pawn_eval = -pawn_eval;
            calculate_pawn_eval(0, WHITE, 10, 20, 80);
            pawn_eval_dirty = 0;
        }

        // Bishop pair: 31 bytes (ae3b5f8 vs 7f7c2b5)
        // 8.0+0.08: 23.84 +- 5.24 (3297 - 2612 - 4091) 0.77 elo/byte
        // 60.0+0.6: 31.91 +- 4.91 (3059 - 2143 - 4698) 1.03 elo/byte
        int e = inc_eval + pawn_eval + BISHOP_PAIR * ((bishops[0] >= 2) - (bishops[1] >= 2));
        // Rook on (semi-)open file: 64 bytes (87a0681 vs 7f7c2b5)
        // 8.0+0.08: 36.62 +- 5.35 (3594 - 2544 - 3862) 0.57 elo/byte
        // 60.0+0.6: 39.82 +- 4.99 (3251 - 2110 - 4639) 0.62 elo/byte
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
        return ((int16_t)stm_eval * phase + (int16_t)(stm_eval + 0x8000 >> 16) * (24 - phase)) / 24;
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
        ROOT.zobrist ^= ZOBRIST.stm;
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
    ROOT.zobrist ^= ZOBRIST.castle_rights[ROOT.castle_rights];
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
    ROOT.zobrist ^= ZOBRIST.castle_rights[ROOT.castle_rights];

    word = strtok(0, " \n");
    if (*word != '-') {
        ROOT.ep_square = (word[1] - '1') * 10 + word[0] - 'a' + A1;
    }

    strtok(0, " \n"); // halfmove clock

    strtok(0, " \n"); // fullmove counter
}
#endif
