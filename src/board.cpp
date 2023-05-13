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
    uint8_t bishops[2];
    uint8_t king_sq[2];
    uint8_t pawn_counts[2][10];
    uint8_t rook_counts[2][8];
    uint8_t ep_square;
    uint8_t castle1, castle2;
    uint8_t stm;
    uint8_t phase;
    int16_t mg_eval;
    int16_t eg_eval;
    uint64_t zobrist;

    void edit(int square, int piece) {
        zobrist ^= ZOBRIST.pieces[board[square]][square-A1];
        if ((board[square] & 7) == ROOK) {
            rook_counts[!(board[square] & WHITE)][square % 10 - 1]--;
        }
        if ((board[square] & 7) == PAWN) {
            pawn_counts[!(board[square] & WHITE)][square % 10]--;
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
                for (int i = STARTS[piece]; i < ENDS[piece]; i++) {
                    int raysq = sq;
                    for (int j = 0; j < LIMITS[piece]; j++) {
                        raysq += RAYS[i];
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

    void pawn_eval(int ci, int color, int pawndir, int zeroth_rank) {
        int shield_pawns = 0;
        int own_pawn = PAWN | color;
        int own_bishop = BISHOP | color;
        int own_knight = KNIGHT | color;
        int own_rook = ROOK | color;
        int own_queen = QUEEN | color;
        int own_king = KING | color;
        int opp_pawn = own_pawn ^ INVALID;
        if (!pawn_counts[ci][king_sq[ci] % 10]) {
            mg_eval += pawn_counts[!ci][king_sq[ci] % 10] ? KING_SEMIOPEN_MG : KING_OPEN_MG;
            eg_eval += pawn_counts[!ci][king_sq[ci] % 10] ? KING_SEMIOPEN_EG : KING_OPEN_EG;
        }
        for (int file = 1; file < 9; file++) {
            if (pawn_counts[ci][file]) {
                mg_eval -= (pawn_counts[ci][file] - 1) * DOUBLED_MG[file - 1];
                eg_eval -= (pawn_counts[ci][file] - 1) * DOUBLED_EG[file - 1];
            } else {
                mg_eval += (pawn_counts[!ci][file] ? ROOK_SEMIOPEN_MG : ROOK_OPEN_MG) * rook_counts[ci][file-1];
                eg_eval += (pawn_counts[!ci][file] ? ROOK_SEMIOPEN_EG : ROOK_OPEN_EG) * rook_counts[ci][file-1];
            }
            if (!pawn_counts[ci][file-1] && !pawn_counts[ci][file+1]) {
                mg_eval -= ISOLATED_PAWN_MG * pawn_counts[ci][file];
                eg_eval -= ISOLATED_PAWN_EG * pawn_counts[ci][file];
            }
            for (int rank = zeroth_rank + 8 * pawndir; rank != zeroth_rank; rank -= pawndir) {
                int sq = file+rank;
                if (board[sq] == own_pawn) {
                    if (king_sq[ci] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    mg_eval += PST[0][own_pawn+6][sq-A1];
                    eg_eval += PST[1][own_pawn+6][sq-A1];
                }
                if (board[file+rank] == opp_pawn || board[file+rank-1] == opp_pawn || board[file+rank+1] == opp_pawn) {
                    break;
                }
            }
            for (int rank = zeroth_rank + 8 * pawndir; rank != zeroth_rank; rank -= pawndir) {
                int sq = rank+file;
                if (board[sq] == own_rook || board[sq] == own_bishop || board[sq] == own_knight || board[sq] == own_king || board[sq] == own_queen) {
                    int mobility = 0;
                    for (int i = STARTS[board[sq] & 7]; i < ENDS[board[sq] & 7]; i++) {
                        int raysq = sq;
                        for (int j = 0; j < LIMITS[board[sq] & 7]; j++) {
                            raysq += RAYS[i];
                            mobility += !(board[raysq] & color);
                            if (board[raysq]) {
                                break;
                            }
                        }
                    }
                    mg_eval += MOBILITY_MG[board[sq] & 7] * mobility;
                    eg_eval += MOBILITY_EG[board[sq] & 7] * mobility;
                }
                if (board[sq] == own_pawn) {
                    int protectors = (board[sq - pawndir + 1] == own_pawn)
                        + (board[sq - pawndir - 1] == own_pawn);
                    mg_eval += PROTECTED_PAWN_MG[protectors];
                    eg_eval += PROTECTED_PAWN_EG[protectors];
                    if (king_sq[ci] % 10 > 4) {
                        sq = 9 + rank - file;
                    }
                    mg_eval += PST[0][own_pawn][sq-A1];
                    eg_eval += PST[1][own_pawn][sq-A1];
                } else if (board[sq] & color) {
                    mg_eval += PST[0][board[sq]][sq-A1];
                    eg_eval += PST[1][board[sq]][sq-A1];
                }
            }
        }
        if (bishops[ci] >= 2) {
            mg_eval += BISHOP_PAIR_MG;
            eg_eval += BISHOP_PAIR_EG;
        }
        for (int dx = -1; dx < 2; dx++) {
            shield_pawns += board[king_sq[ci]+dx+pawndir] == own_pawn
                || board[king_sq[ci]+dx+pawndir*2] == own_pawn;
        }
        mg_eval +=
            (king_sq[ci] / 10 == zeroth_rank / 10 + pawndir / 10) * PAWN_SHIELD_MG[shield_pawns] +
            TEMPO_MG * (color == stm);
        eg_eval +=
            (king_sq[ci] / 10 == zeroth_rank / 10 + pawndir / 10) * PAWN_SHIELD_EG[shield_pawns] +
            TEMPO_EG * (color == stm);
    }

    int eval() {
        mg_eval = 0;
        eg_eval = 0;
        pawn_eval(1, BLACK, -10, 100);
        mg_eval = -mg_eval;
        eg_eval = -eg_eval;
        pawn_eval(0, WHITE, 10, 10);

        int value = (mg_eval * phase + eg_eval * (24 - phase)) / 24;
        return stm == WHITE ? value : -value;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
