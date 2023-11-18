struct Move {
    uint8_t from: 7;
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
    uint8_t pawn_eval_dirty;
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
        castle1 = 0;
        castle2 = 0;
    }

    void remove_castle_rights(int btm, int which) {
        if (castle_rights[btm] & which) {
            castle_rights[btm] &= ~which;
            zobrist ^= ZOBRIST.castle_rights[which ^ btm];
        }
    }

    void make_move(Move mv, int promo = QUEEN) {
        int piece = mv.promo ? promo | stm : board[mv.from];
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

    int movegen(Move list[], int& count, int quiets, int& mobility) {
        count = 0;
        mobility = 0;
        uint8_t other = stm ^ INVALID;
        uint8_t opponent_king = other | KING;
        for (int sq = A1; sq <= H8; sq++) {
            // skip empty squares & opponent squares (& border squares)
            if ((board[sq] & 0x18) != stm) {
                continue;
            }

            int rays[] = {-1, 1, -10, 10, 11, -11, 9, -9, -21, 21, -19, 19, -12, 12, -8, 8};
            int starts[] = {0,0,8,4,0,0,0};
            int limits[] = {0,0,1,8,8,8,1};
            int ends[] = {0,0,16,8,4,8,8};
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
                if (
                    board[sq + dir+1] == opponent_king || board[sq + dir-1] == opponent_king ||
                    sq + dir+1 == castle1 || sq + dir+1 == castle2 ||
                    sq + dir-1 == castle1 || sq + dir-1 == castle2
                ) {
                    return 0;
                }
                if (ep_square == sq + dir-1 || board[sq + dir-1] & other && ~board[sq + dir-1] & stm) {
                    mobility += MOBILITY[piece];
                    list[count++] = Move(sq, sq + dir-1, promo);
                }
                if (ep_square == sq + dir+1 || board[sq + dir+1] & other && ~board[sq + dir+1] & stm) {
                    mobility += MOBILITY[piece];
                    list[count++] = Move(sq, sq + dir+1, promo);
                }
            } else {
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
                        mobility += MOBILITY[piece];
                        if (board[raysq] & other) {
                            list[count++] = Move(sq, raysq);
                            break;
                        } else if (quiets) {
                            list[count++] = Move(sq, raysq);
                        }
                    }
                }
            }
        }
        return 1;
    }

    void calculate_pawn_eval(int ci, int color, int pawndir, int first_rank, int seventh_rank) {
        int shield_pawns = 0;
        int own_pawn = PAWN | color;
        int opp_pawn = own_pawn ^ INVALID;
        if (!pawn_counts[ci][king_sq[ci] % 10]) {
            pawn_eval += pawn_counts[!ci][king_sq[ci] % 10] ? KING_SEMIOPEN : KING_OPEN;
        }
        for (int file = 1; file < 9; file++) {
            // Doubled pawns: 44 bytes (8117455 vs 7f7c2b5)
            // 8.0+0.08: 5.04 +- 5.14 (2930 - 2785 - 4285) 0.11 elo/byte
            // 60.0+0.6: 6.46 +- 4.69 (2473 - 2287 - 5240) 0.15 elo/byte
            if (pawn_counts[ci][file]) {
                pawn_eval -= (pawn_counts[ci][file] - 1) * DOUBLED_PAWN[file - 1];
            }
            // Isolated pawns: 18 bytes (b4d32e5 vs 7f7c2b5)
            // 8.0+0.08: 14.64 +- 5.20 (3128 - 2707 - 4165) 0.81 elo/byte
            // 60.0+0.6: 16.79 +- 4.82 (2749 - 2266 - 4985) 0.93 elo/byte
            if (!pawn_counts[ci][file-1] && !pawn_counts[ci][file+1]) {
                pawn_eval -= ISOLATED_PAWN * pawn_counts[ci][file];
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
                    int protectors = (board[sq - pawndir + 1] == own_pawn)
                        + (board[sq - pawndir - 1] == own_pawn);
                    pawn_eval += PROTECTED_PAWN[protectors];
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
        int bishop_pair = (bishops[0] >= 2) - (bishops[1] >= 2);
        int e = inc_eval + pawn_eval + BISHOP_PAIR * bishop_pair;
        // Rook on (semi-)open file: 64 bytes (87a0681 vs 7f7c2b5)
        // 8.0+0.08: 36.62 +- 5.35 (3594 - 2544 - 3862) 0.57 elo/byte
        // 60.0+0.6: 39.82 +- 4.99 (3251 - 2110 - 4639) 0.62 elo/byte
        for (int file = 1; file < 9; file++) {
            if (!pawn_counts[0][file]) {
                e += (pawn_counts[1][file] ? ROOK_SEMIOPEN : ROOK_OPEN) * rook_counts[0][file-1];
            }
            if (!pawn_counts[1][file]) {
                e -= (pawn_counts[0][file] ? ROOK_SEMIOPEN : ROOK_OPEN) * rook_counts[1][file-1];
            }
        }
        stm_eval += stm == WHITE ? e : -e;
        return ((int16_t)stm_eval * phase + (int16_t)(stm_eval + 0x8000 >> 16) * (24 - phase)) / 24;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
