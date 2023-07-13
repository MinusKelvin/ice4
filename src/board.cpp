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
            putchar('q');
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
    uint8_t board[120];
    uint8_t castle_rights[2];
    uint8_t ep_square;
    uint8_t castle1, castle2;
    uint8_t stm;
    int32_t inc_eval;
    uint64_t zobrist;

    void edit(int square, int piece) {
        zobrist ^= ZOBRIST.pieces[board[square]][square-A1];
        inc_eval -= PST[board[square]][square-A1];
        board[square] = piece;
        zobrist ^= ZOBRIST.pieces[board[square]][square-A1];
        inc_eval += PST[board[square]][square-A1];
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
        castle1 = 0;
        castle2 = 0;
    }

    void remove_castle_rights(int btm, int which) {
        if (castle_rights[btm] & which) {
            castle_rights[btm] &= ~which;
            zobrist ^= ZOBRIST.castle_rights[which ^ btm];
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
        zobrist ^= ZOBRIST.stm;
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
            int piece = board[sq] & 7;

            if (piece == KING && sq == (stm == WHITE ? E1 : E8) && quiets) {
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
                int dir = stm == WHITE ? 10 : -10;
                int upsq = sq + dir;
                int promo = board[upsq + dir] == INVALID ? QUEEN : 0;
                if (!board[upsq] && (quiets || promo || board[upsq + dir + dir] == INVALID)) {
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
            } else {
                int starts[] = {0,0,8,4,0,0,0};
                int limits[] = {0,0,1,8,8,8,1};
                int ends[] = {0,0,16,8,4,8,8};

                for (int i = starts[piece]; i < ends[piece]; i++) {
                    int raysq = sq;
                    for (int j = 0; j < limits[piece]; j++) {
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

    int eval() {
        return stm == WHITE ? inc_eval : -inc_eval;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
