struct Move {
    uint8_t from;
    uint8_t to : 7;
    uint8_t promo : 1;

    Move() = default;
    Move(uint8_t f, uint8_t t=0, uint8_t p=0) : from(f), to(t), promo(p) {}

    void put_with_newline() {
        putchar(from%10+96);
        putchar(from/10+47);
        putchar(to%10+96);
        putchar(to/10+47);
        if (promo) {
            putchar('q');
        }
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
    uint8_t castle_rights[2];
    uint8_t bishops[2];
    uint8_t king_sq[2];
    uint8_t pawn_counts[2][10];
    uint8_t rook_counts[2][8];
    uint8_t ep_square;
    uint8_t stm;
    uint8_t phase;
    uint8_t pawn_eval_dirty;
    uint8_t check;
    int32_t inc_eval;
    int32_t pawn_eval;
    uint64_t zobrist;

    void edit(int square, int piece) {
        if ((board[square] & 7) == PAWN || (piece & 7) == PAWN || (piece & 7) == KING) {
            pawn_eval_dirty = 1;
        }
        zobrist ^= ZOBRIST.pieces[board[square]][square-A1];
        if ((board[square] & 7) == ROOK) {
            rook_counts[!(board[square] & WHITE)][square % 10 - 1]--;
        }
        if ((board[square] & 7) == PAWN) {
            pawn_counts[!(board[square] & WHITE)][square % 10]--;
        } else {
            inc_eval -= PST[board[square]][square-A1];
        }
        phase -= PHASE[board[square] & 7];
        if ((board[square] & 7) == BISHOP) {
            bishops[!(board[square] & WHITE)]--;
        }
        board[square] = piece;
        zobrist ^= ZOBRIST.pieces[board[square]][square-A1];
        if ((board[square] & 7) == ROOK) {
            rook_counts[!(board[square] & WHITE)][square % 10 - 1]++;
        }
        if ((board[square] & 7) == PAWN) {
            pawn_counts[!(board[square] & WHITE)][square % 10]++;
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
        castle_rights[0] = 3;
        castle_rights[1] = 3;
        stm = WHITE;
        int layout[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            edit(A1 + i, layout[i] | WHITE);
            edit(A8 + i, layout[i] | BLACK);
            edit(A2 + i, PAWN | WHITE);
            edit(A7 + i, PAWN | BLACK);
            edit(A3 + i, EMPTY);
            edit(A4 + i, EMPTY);
            edit(A5 + i, EMPTY);
            edit(A6 + i, EMPTY);
        }
    }

    void null_move() {
        zobrist ^= ZOBRIST.stm;
        stm ^= INVALID;
        ep_square = 0;
    }

    void remove_castle_rights(int btm, int which) {
        if (castle_rights[btm] & which) {
            castle_rights[btm] &= ~which;
            zobrist ^= ZOBRIST.castle_rights[which ^ btm];
        }
    }

    int attacked(int ksq, int by) {
        int pawndir = by & WHITE ? -10 : 10;
        if (board[ksq + pawndir + 1] == (PAWN | by) || board[ksq + pawndir - 1] == (PAWN | by)) {
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
        int nstm = stm ^ INVALID;
        int btm = stm != WHITE;
        edit(mv.to, piece);
        edit(mv.from, EMPTY);

        // handle en-passant
        zobrist ^= ZOBRIST.ep[ep_square];
        int ep = btm ? 10 : -10;
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
        int back_rank = btm ? A8 : A1;
        if ((piece & 7) == KING && mv.from == back_rank + 4) {
            if (mv.to == back_rank + 6) {
                edit(back_rank + 7, EMPTY);
                edit(back_rank + 5, stm | ROOK);
                if (attacked(back_rank + 4, nstm) || attacked(back_rank + 5, nstm)) {
                    return 1;
                }
            }
            if (mv.to == back_rank + 2) {
                edit(back_rank + 0, EMPTY);
                edit(back_rank + 3, stm | ROOK);
                if (attacked(back_rank + 4, nstm) || attacked(back_rank + 3, nstm)) {
                    return 1;
                }
            }
            remove_castle_rights(btm, SHORT_CASTLE);
            remove_castle_rights(btm, LONG_CASTLE);
        }

        if (mv.from == A1 || mv.to == A1) {
            remove_castle_rights(0, LONG_CASTLE);
        }
        if (mv.from == H1 || mv.to == H1) {
            remove_castle_rights(0, SHORT_CASTLE);
        }
        if (mv.from == A8 || mv.to == A8) {
            remove_castle_rights(1, LONG_CASTLE);
        }
        if (mv.from == H8 || mv.to == H8) {
            remove_castle_rights(1, SHORT_CASTLE);
        }

        if (attacked(king_sq[btm], nstm)) {
            return 1;
        }

        check = attacked(king_sq[!btm], stm);
        stm ^= INVALID;
        zobrist ^= ZOBRIST.stm;
        return 0;
    }

    int movegen_and_eval(Move list[], int& count, int quiets) {
        int square_flags[120] = {};
        int eval = 0;
        count = 0;
        for (int sq = A1; sq <= H8; sq++) {
            int piece = board[sq] & 7;
            int color = board[sq] & INVALID;
            int other = color ^ INVALID;
            int moving = color == stm;
            int mob_sign = color == WHITE ? 1 : -1;
            int attacker_bit = 1 << board[sq] * 2 - 16;

            // pull down pawn behindness and aheadness
            int flip_sq = (A1+H8) - sq;
            square_flags[sq] |= square_flags[sq-10] & (WHITE_PAWN_AHEAD | BLACK_PAWN_BEHIND);
            square_flags[flip_sq] |= square_flags[flip_sq+10] & (WHITE_PAWN_BEHIND | BLACK_PAWN_AHEAD);
            square_flags[sq+10] |= WHITE_PAWN_AHEAD * (board[sq] == WHITE_PAWN);
            square_flags[sq] |= BLACK_PAWN_BEHIND * (board[sq] == BLACK_PAWN);
            square_flags[flip_sq] |= WHITE_PAWN_BEHIND * (board[flip_sq] == WHITE_PAWN);
            square_flags[flip_sq-10] |= BLACK_PAWN_AHEAD * (board[flip_sq] == BLACK_PAWN);

            // no movegen for empty squares (& border squares)
            if (!(board[sq] & 7)) {
                continue;
            }

            if (piece == KING && sq == (stm == WHITE ? E1 : E8) && quiets && moving) {
                if (castle_rights[stm == BLACK] & SHORT_CASTLE &&
                        !board[sq+1] && !board[sq+2]) {
                    list[count++] = Move(sq, sq + 2, 0);
                }
                if (castle_rights[stm == BLACK] & LONG_CASTLE &&
                        !board[sq-1] && !board[sq-2] && !board[sq-3]) {
                    list[count++] = Move(sq, sq - 2, 0);
                }
            }

            if (piece == PAWN) {
                int dir = color == WHITE ? 10 : -10;
                int promo = board[sq + dir + dir] == INVALID;
                if (!board[sq + dir]) {
                    eval += mob_sign * MOBILITY[piece];
                    if ((quiets || promo || board[sq + dir + dir + dir] == INVALID) && moving) {
                        list[count++] = Move(sq, sq + dir, promo);
                    }
                    if (board[sq - dir - dir] == INVALID && !board[sq + dir + dir]) {
                        eval += mob_sign * MOBILITY[piece];
                        if (quiets && moving) {
                            list[count++] = Move(sq, sq + dir+dir, promo);
                        }
                    }
                }
                square_flags[sq + dir-1] |= square_flags[sq + dir-1] + attacker_bit;
                square_flags[sq + dir+1] |= square_flags[sq + dir+1] + attacker_bit;
                if (ep_square == sq + dir-1 || board[sq + dir-1] & other && ~board[sq + dir-1] & color) {
                    eval += mob_sign * MOBILITY[piece];
                    if (moving) {
                        list[count++] = Move(sq, sq + dir-1, promo);
                    }
                }
                if (ep_square == sq + dir+1 || board[sq + dir+1] & other && ~board[sq + dir+1] & color) {
                    eval += mob_sign * MOBILITY[piece];
                    if (moving) {
                        list[count++] = Move(sq, sq + dir+1, promo);
                    }
                }
            } else {
                for (int i = STARTS[piece]; i < ENDS[piece]; i++) {
                    int raysq = sq;
                    for (int j = 0; j < LIMITS[piece]; j++) {
                        raysq += RAYS[i];
                        if (board[raysq] == INVALID) {
                            break;
                        }
                        square_flags[raysq] |= square_flags[raysq] + attacker_bit;
                        if (board[raysq] & color) {
                            break;
                        }
                        eval += mob_sign * MOBILITY[piece];
                        if (board[raysq] & other) {
                            if (moving) {
                                list[count++] = Move(sq, raysq);
                            }
                            break;
                        } else if (quiets && moving) {
                            list[count++] = Move(sq, raysq);
                        }
                    }
                }
            }
        }

        for (int rank = 0; rank < 8; rank++) {
            for (int file = 0; file < 8; file++) {
                int sq = rank*10 + file + A1;
                int piece = board[sq] & 7;
                int sign = board[sq] & WHITE ? 1 : -1;
                int flipped_rank = board[sq] & WHITE ? rank : 7 - rank;
                int own_flags = board[sq] & WHITE ? 0 : 16;
                int opp_flags = 16 - own_flags;

                if (square_flags[sq] & 0x3000) {
                    int attacks = __builtin_popcount(square_flags[sq] >> 16 & 0xFFFC);
                    eval -= KING_ATTACKS * attacks;
                    eval -= DOUBLE_KING_ATTACKS * (attacks >= 2);
                }
                if (square_flags[sq] >> 16 & 0x3000) {
                    int attacks = __builtin_popcount(square_flags[sq] & 0xFFFC);
                    eval += KING_ATTACKS * attacks;
                    eval += DOUBLE_KING_ATTACKS * (attacks >= 2);
                }

                if (!piece) {
                    continue;
                }

                if (piece == PAWN && !(
                    square_flags[sq-1] >> opp_flags & PAWN_AHEAD ||
                    square_flags[sq] >> opp_flags & PAWN_AHEAD ||
                    square_flags[sq+1] >> opp_flags & PAWN_AHEAD
                )) {
                    eval += sign * PASSED_PAWN[flipped_rank];
                }

                if (piece == PAWN && !(
                    square_flags[sq-1] >> own_flags & 3 ||
                    square_flags[sq+1] >> own_flags & 3
                )) {
                    eval += sign * ISOLATED_PAWN;
                }

                if (piece == PAWN && square_flags[sq] >> own_flags & PAWN_AHEAD) {
                    eval += sign * DOUBLED_PAWN;
                }

                if (square_flags[sq] >> opp_flags & (1 << piece*2) - 4) {
                    eval += sign * THREATENED[piece];
                }

                if (piece == ROOK && square_flags[sq] >> own_flags & PAWN_BEHIND) {
                    eval += sign * ROOK_BEHIND_PAWN;
                }

                eval += sign * PIECE_RANK[(piece-1) * 8 + flipped_rank];
                eval += sign * PIECE_FILE[(piece-1) * 8 + file];
            }
        }

        eval *= stm & WHITE ? 1 : -1;
        eval += TEMPO;

        return ((int16_t)eval * phase + (eval + 0x8000 >> 16) * (24 - phase)) / 24;
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
    if (remove_white_short) {
        ROOT.remove_castle_rights(0, SHORT_CASTLE);
    }
    if (remove_white_long) {
        ROOT.remove_castle_rights(0, LONG_CASTLE);
    }
    if (remove_black_short) {
        ROOT.remove_castle_rights(1, SHORT_CASTLE);
    }
    if (remove_black_long) {
        ROOT.remove_castle_rights(1, LONG_CASTLE);
    }

    word = strtok(0, " \n");
    if (*word != '-') {
        ROOT.ep_square = (word[1] - '1') * 10 + word[0] - 'a' + A1;
    }

    strtok(0, " \n"); // halfmove clock

    strtok(0, " \n"); // fullmove counter
}
#endif
