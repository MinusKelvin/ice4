struct Move {
    int8_t from;
    int8_t to;
    int8_t promo;

    Move() = default;
    Move(int8_t f, int8_t t=0, int8_t p=0) : from(f), to(t), promo(p) {}
    auto operator==(Move& r) {
        return from == r.from && to == r.to && promo == r.promo;
    }

    void put() {
        putchar(from%10+'a'-1);
        putchar(from/10+'0'-1);
        putchar(to%10+'a'-1);
        putchar(to/10+'0'-1);
        if (promo) {
            putchar(
                promo == QUEEN ? 'q' :
                promo == ROOK ? 'r' :
                promo == BISHOP ? 'b' : 'n'
            );
        }
    }
};

struct TtData {
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;
    uint8_t padding;
};

struct TtEntry {
    atomic_uint64_t hash_xor_data;
    atomic_uint64_t data;

    TtEntry() : hash_xor_data(0), data(0) {}
};

// 8MB. Replaced for TCEC builds by the minifier.
#define HASH_SIZE 524288
vector<TtEntry> TT(HASH_SIZE);

struct Board {
    uint64_t zobrist;
    uint8_t board[120];
    uint8_t castle_rights[2];
    uint8_t ep_square;
    uint8_t castle1, castle2;
    uint8_t stm;
    uint8_t phase;
    uint8_t pawn_eval_dirty;
    uint8_t bishops[2];
    uint8_t king_sq[2];
    uint8_t pawn_counts[2][10];
    int16_t mg_eval;
    int16_t eg_eval;
    int16_t mg_pawn_eval;
    int16_t eg_pawn_eval;

    Board() : zobrist(0), castle_rights{3,3}, ep_square(0), castle1(0), castle2(0), stm(WHITE),
        phase(24), pawn_eval_dirty(1), bishops{2, 2}, king_sq{E1, E8}, pawn_counts{}, mg_eval(0),
        eg_eval(0), mg_pawn_eval(0), eg_pawn_eval(0)
    {
        memset(board, INVALID, 120);
        memset(pawn_counts, 0, sizeof(pawn_counts));
        int layout[] = { ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK };
        for (int i = 0; i < 8; i++) {
            board[A1 + i] = layout[i] | WHITE;
            board[A2 + i] = PAWN | WHITE;
            board[A3 + i] = EMPTY;
            board[A4 + i] = EMPTY;
            board[A5 + i] = EMPTY;
            board[A6 + i] = EMPTY;
            board[A7 + i] = PAWN | BLACK;
            board[A8 + i] = layout[i] | BLACK;
            pawn_counts[0][i+1] = pawn_counts[1][i+1] = 1;
        }
    }

    void edit(int square, int piece) {
        if ((board[square] & 7) == PAWN || (piece & 7) == PAWN || (piece & 7) == KING) {
            pawn_eval_dirty = 1;
        }
        zobrist ^= ZOBRIST_PIECES[board[square]][square-A1];
        if ((board[square] & 7) == PAWN) {
            pawn_counts[!(board[square] & WHITE)][square % 10]--;
        } else {
            mg_eval -= PST[0][board[square]][square-A1];
            eg_eval -= PST[1][board[square]][square-A1];
        }
        phase -= PHASE[board[square] & 7];
        if ((board[square] & 7) == BISHOP) {
            bishops[!(board[square] & WHITE)]--;
        }
        board[square] = piece;
        zobrist ^= ZOBRIST_PIECES[board[square]][square-A1];
        if ((board[square] & 7) == PAWN) {
            pawn_counts[!(board[square] & WHITE)][square % 10]++;
        } else {
            mg_eval += PST[0][board[square]][square-A1];
            eg_eval += PST[1][board[square]][square-A1];
        }
        phase += PHASE[board[square] & 7];
        if ((board[square] & 7) == BISHOP) {
            bishops[!(board[square] & WHITE)]++;
        }
        if ((board[square] & 7) == KING) {
            king_sq[!(board[square] & WHITE)] = square;
        }
    }

    void null_move() {
        zobrist ^= ZOBRIST_STM;
        stm ^= INVALID;
        ep_square = 0;
        castle1 = 0;
        castle2 = 0;
    }

    void remove_castle_rights(int btm, int which) {
        if (castle_rights[btm] & which) {
            castle_rights[btm] &= ~which;
            zobrist ^= ZOBRIST_CASTLE_RIGHTS[which ^ btm];
        }
    }

    void make_move(Move mv) {
        int piece = mv.promo ? mv.promo | stm : board[mv.from];
        int btm = stm != WHITE;
        castle1 = 0;
        castle2 = 0;
        edit(mv.to, piece);
        edit(mv.from, EMPTY);

        // handle en-passant
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

        // handle castling
        int back_rank = btm ? A8 : A1;
        if ((piece & 7) == KING && mv.from == back_rank + 4) {
            if (mv.to == back_rank + 6) {
                edit(back_rank + 7, EMPTY);
                edit(back_rank + 5, stm | ROOK);
                castle1 = back_rank + 4;
                castle2 = back_rank + 5;
            }
            if (mv.to == back_rank + 2) {
                edit(back_rank + 0, EMPTY);
                edit(back_rank + 3, stm | ROOK);
                castle1 = back_rank + 4;
                castle2 = back_rank + 3;
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

        stm ^= INVALID;
        zobrist ^= ZOBRIST_STM;
    }

    int movegen(Move list[], int& count, int quiets=1) {
        count = 0;
        uint8_t other = stm ^ INVALID;
        uint8_t opponent_king = other | KING;
        for (int sq = A1; sq <= H8; sq++) {
            // skip empty squares & opponent squares (& border squares)
            if (!board[sq] || board[sq] & other) {
                continue;
            }

            int rays[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
            int8_t limits[16] = {0};
            int piece = board[sq] & 7;

            if (piece == KING && sq == (stm == WHITE ? E1 : E8)) {
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
                int orig = count; // remember start of pawn move list for underpromotions

                int dir = stm == WHITE ? 10 : -10;
                int upsq = sq + dir;
                int promo = board[upsq + dir] == INVALID ? stm | QUEEN : 0;
                if (!board[upsq] && quiets) {
                    list[count++] = Move(sq, upsq, promo);
                    if (board[sq - dir - dir] == INVALID && !board[upsq+dir]) {
                        list[count++] = Move(sq, upsq+dir, promo);
                    }
                }
                if (
                    board[upsq+1] == opponent_king || board[upsq-1] == opponent_king ||
                    upsq+1 == castle1 || upsq+1 == castle2 ||
                    upsq-1 == castle1 || upsq-1 == castle2
                ) {
                    return 0;
                }
                if (ep_square == upsq-1 || board[upsq-1] & other && ~board[upsq-1] & stm) {
                    list[count++] = Move(sq, upsq-1, promo);
                }
                if (ep_square == upsq+1 || board[upsq+1] & other && ~board[upsq+1] & stm) {
                    list[count++] = Move(sq, upsq+1, promo);
                }

                // copy in underpromotion moves
                int end = count;
                for (int i = orig; promo && i < end; i++) {
                    list[count] = list[i];
                    list[count++].promo = ROOK;
                    list[count] = list[i];
                    list[count++].promo = BISHOP;
                    list[count] = list[i];
                    list[count++].promo = KNIGHT;
                }
            } else {
                int starts[] = {0,0,8,4,0,0,0};
                int vals[] = {0,0,1,8,8,8,1};
                int lens[] = {0,0,8,4,4,8,8};
                memset(limits+starts[piece], vals[piece], lens[piece]);

                for (int i = 0; i < 16; i++) {
                    int raysq = sq;
                    for (int j = 0; j < limits[i]; j++) {
                        raysq += rays[i];
                        if (board[raysq] & stm) {
                            break;
                        }
                        if (board[raysq] == opponent_king || raysq == castle1 || raysq == castle2) {
                            return 0;
                        }
                        if (board[raysq] & other) {
                            list[count++] = Move(sq, raysq, 0);
                            break;
                        } else if (quiets) {
                            list[count++] = Move(sq, raysq, 0);
                        }
                    }
                }
            }
        }
        return 1;
    }

    void update_pawn_eval() {
        eg_pawn_eval = 0;
        mg_pawn_eval = 0;
        for (int file = 1; file < 9; file++) {
            if (pawn_counts[0][file]) {
                mg_pawn_eval += (pawn_counts[0][file] - 1) * DOUBLED_MG[file-1];
                eg_pawn_eval += (pawn_counts[0][file] - 1) * DOUBLED_EG[file-1];
            }
            if (pawn_counts[1][file]) {
                mg_pawn_eval -= (pawn_counts[1][file] - 1) * DOUBLED_MG[file-1];
                eg_pawn_eval -= (pawn_counts[1][file] - 1) * DOUBLED_EG[file-1];
            }
            for (int rank = 30; rank < 90; rank += 10) {
                int sq = file+rank;
                if (board[sq] == BLACK_PAWN) {
                    if (king_sq[1] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    mg_pawn_eval += PST[0][BLACK_PASSED_PAWN][sq-A1];
                    eg_pawn_eval += PST[1][BLACK_PASSED_PAWN][sq-A1];
                }
                if (board[file+rank] == WHITE_PAWN || board[file+rank-1] == WHITE_PAWN || board[file+rank+1] == WHITE_PAWN) {
                    break;
                }
            }
            for (int rank = 80; rank >= 30; rank -= 10) {
                int sq = file+rank;
                if (board[sq] == WHITE_PAWN) {
                    if (king_sq[0] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    mg_pawn_eval += PST[0][WHITE_PASSED_PAWN][sq-A1];
                    eg_pawn_eval += PST[1][WHITE_PASSED_PAWN][sq-A1];
                }
                if (board[file+rank] == BLACK_PAWN || board[file+rank-1] == BLACK_PAWN || board[file+rank+1] == BLACK_PAWN) {
                    break;
                }
            }
        }
        for (int rank = 30; rank < 90; rank += 10) {
            for (int file = 1; file < 9; file++) {
                int sq = rank+file;
                int piece = board[sq];
                if ((piece & 7) == PAWN) {
                    if (king_sq[!(piece & WHITE)] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    mg_pawn_eval += PST[0][piece][sq-A1];
                    eg_pawn_eval += PST[1][piece][sq-A1];
                }
            }
        }
        if (pawn_counts[0][king_sq[0] % 10]) {
            mg_pawn_eval += KING_OPEN_FILE_MG;
            eg_pawn_eval += KING_OPEN_FILE_EG;
        }
        if (pawn_counts[1][king_sq[1] % 10]) {
            mg_pawn_eval -= KING_OPEN_FILE_MG;
            eg_pawn_eval -= KING_OPEN_FILE_EG;
        }
    }

    int eval() {
        if (pawn_eval_dirty) {
            update_pawn_eval();
            pawn_eval_dirty = 0;
        }
        int bishop_pair = (bishops[0] >= 2) - (bishops[1] >= 2);
        int mg = mg_eval + mg_pawn_eval + BISHOP_PAIR_MG * bishop_pair;
        int eg = eg_eval + eg_pawn_eval + BISHOP_PAIR_EG * bishop_pair;
        int value = (mg * phase + eg * (24 - phase)) / 24;
        return stm == WHITE ? value : -value;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
