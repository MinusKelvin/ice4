struct Move {
    int8_t from;
    int8_t to;
    int8_t promo;

    Move(int8_t f=0, int8_t t=0, int8_t p=0) : from(f), to(t), promo(p) {}
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

struct TtEntry {
    uint64_t hash;
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;

    TtEntry() : hash(~0) {}
};

std::vector<TtEntry> TT(524288); // 8MB

struct Board {
    uint8_t board[120];
    uint64_t zobrist;
    // uint8_t castle_rights[2];
    uint8_t ep_square;
    uint8_t stm;
    uint8_t halfmove;
    uint8_t phase;
    int16_t mg_eval;
    int16_t eg_eval;

    Board() : zobrist(0), /*castle_rights{3,3},*/ ep_square(0), stm(WHITE), halfmove(0),
            phase(24), mg_eval(0), eg_eval(0)
    {
        memset(board, INVALID, 120);
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
        }
    }

    void edit(int square, int piece) {
        zobrist ^= ZOBRIST_PIECES[board[square]][square-A1];
        mg_eval -= PST[0][board[square]][square-A1];
        eg_eval -= PST[1][board[square]][square-A1];
        phase -= PHASE[board[square] & 7];
        board[square] = piece;
        zobrist ^= ZOBRIST_PIECES[board[square]][square-A1];
        mg_eval += PST[0][board[square]][square-A1];
        eg_eval += PST[1][board[square]][square-A1];
        phase += PHASE[board[square] & 7];
    }

    void null_move() {
        zobrist ^= ZOBRIST_STM;
        stm ^= INVALID;
        ep_square = 0;
        halfmove++;
    }

    void make_move(Move mv) {
        int piece = mv.promo ? mv.promo | stm : board[mv.from];
        int btm = stm != WHITE;
        halfmove++;
        if (piece == PAWN || board[mv.to]) {
            halfmove = 0;
        }
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
                // castle_rights[btm] = 0;
            }
            if (mv.to == back_rank + 2) {
                edit(back_rank + 0, EMPTY);
                edit(back_rank + 3, stm | ROOK);
                // castle_rights[btm] = 0;
            }
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
            if (!board[sq] || board[sq] & other) continue;

            int rays[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
            int8_t limits[16] = {0};
            int piece = board[sq] & 7;
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
                if (board[upsq+1] == opponent_king || board[upsq-1] == opponent_king) {
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
                    list[count++].promo = stm | ROOK;
                    list[count] = list[i];
                    list[count++].promo = stm | BISHOP;
                    list[count] = list[i];
                    list[count++].promo = stm | KNIGHT;
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
                        if (board[raysq] & stm) break;
                        if (board[raysq] == opponent_king) return 0;
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
        int value = (mg_eval * phase + eg_eval * (24 - phase)) / 24;
        return stm == WHITE ? value : -value;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
