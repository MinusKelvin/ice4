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
            putchar("  nbrq"[promo]);
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

uint64_t n(uint64_t bb) {
    return bb << 8;
}

uint64_t s(uint64_t bb) {
    return bb >> 8;
}

uint64_t w(uint64_t bb) {
    return bb << 1 & 0x7F7F7F7F7F7F7F7Full;
}

uint64_t e(uint64_t bb) {
    return bb >> 1 & 0xFEFEFEFEFEFEFEFEull;
}

struct Board {
    uint64_t pieces[6];
    uint64_t colors[2];
    uint8_t castle_rights[2];
    uint8_t bishops[2];
    uint8_t king_sq[2];
    uint8_t pawn_counts[2][10];
    uint8_t ep_square;
    uint8_t castle1, castle2;
    uint8_t stm;
    uint8_t phase;
    uint8_t pawn_eval_dirty;
    int16_t mg_eval;
    int16_t eg_eval;
    int16_t mg_pawn_eval;
    int16_t eg_pawn_eval;
    uint64_t zobrist;

    Board() : zobrist(0), castle_rights{3,3}, ep_square(0), castle1(64), castle2(64), stm(WHITE),
        phase(24), pawn_eval_dirty(1), bishops{2, 2}, king_sq{E1, E8}, mg_eval(0), eg_eval(0),
        mg_pawn_eval(0), eg_pawn_eval(0)
    {
        memset(board, INVALID, 120);
        memset(pawn_counts, 1, sizeof(pawn_counts));
        pawn_counts[WHITE][0] = 0;
        pawn_counts[WHITE][9] = 0;
        pawn_counts[BLACK][0] = 0;
        pawn_counts[BLACK][9] = 0;
        pieces[PAWN-1] = 0x00FF00000000FF00ull;
        pieces[KNIGHT-1] = 0x4200000000000042ull;
        pieces[BISHOP-1] = 0x2400000000000024ull;
        pieces[ROOK-1] = 0x8100000000000081ull;
        pieces[QUEEN-1] = 0x0800000000000008ull;
        pieces[KING-1] = 0x1000000000000010ull;
        colors[WHITE] = 0xFFFFull;
        colors[BLACK] = 0xFFFF000000000000ull;
    }

    int piece_on(int square) {
        for (int i = 0; i < 6; i++) {
            if (pieces[i] & (1 << square)) {
                return i+1;
            }
        }
        return 0;
    }

    void edit(int square, int color, int piece) {
        int old_piece = piece_on(square);
        int old_color = !(colors[WHITE] & (1 << square));
        if (old_piece == PAWN || piece == PAWN || piece == KING) {
            pawn_eval_dirty = 1;
        }

        zobrist ^= ZOBRIST_PIECES[old_color][old_piece][square];
        if (old == PAWN) {
            pawn_counts[old_color][square % 8 + 1]--;
        } else {
            mg_eval -= PST[0][old_color][old_piece][square];
            eg_eval -= PST[1][old_color][old_piece][square];
        }
        phase -= PHASE[old_piece];
        if (old_piece == BISHOP) {
            bishops[old_color]--;
        }

        if (old_piece) {
            colors[old_color] ^= 1ull << square;
            pieces[old_piece-1] ^= 1ull << square;
        }
        if (piece) {
            colors[color] ^= 1ull << square;
            pieces[piece-1] ^= 1ull << square;
        }

        zobrist ^= ZOBRIST_PIECES[color][piece][square];
        if (piece == PAWN) {
            pawn_counts[color][square % 8 + 1]++;
        } else {
            mg_eval += PST[0][color][piece][square];
            eg_eval += PST[1][color][piece][square];
        }
        phase += PHASE[piece];
        if (piece == BISHOP) {
            bishops[color]++;
        }
        if (piece == KING) {
            king_sq[color] = square;
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
        int piece = mv.promo ? mv.promo : piece_on(mv.from);
        castle1 = 0;
        castle2 = 0;
        edit(mv.to, stm, piece);
        edit(mv.from, 0, EMPTY);

        // handle en-passant
        int ep = stm ? 8 : -8;
        if (piece == PAWN) {
            if (mv.to == ep_square) {
                edit(mv.to + ep, 0, EMPTY);
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
        int back_rank = stm ? A8 : A1;
        if (piece == KING && mv.from == back_rank + 4) {
            if (mv.to == back_rank + 6) {
                edit(back_rank + 7, 0, EMPTY);
                edit(back_rank + 5, stm, ROOK);
                castle1 = back_rank + 4;
                castle2 = back_rank + 5;
            }
            if (mv.to == back_rank + 2) {
                edit(back_rank + 0, 0, EMPTY);
                edit(back_rank + 3, stm, ROOK);
                castle1 = back_rank + 4;
                castle2 = back_rank + 3;
            }
            remove_castle_rights(stm, SHORT_CASTLE);
            remove_castle_rights(stm, LONG_CASTLE);
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

        stm ^= 1;
        zobrist ^= ZOBRIST_STM;
    }

    int movegen(Move list[], int& count, int quiets=1) {
        count = 0;
        uint64_t occupied = colors[0] | colors[1];

        // pawns
        uint64_t own_pawns = piece[PAWN-1] & colors[stm];
        int dir = stm ? -8 : 8;
        for (uint64_t bb = (stm ? s(own_pawns) : n(own_pawns)) & ~occupied; bb; bb &= bb-1) {
            int sq = countr_zero(bb);
            list[count++] = Move(sq - dir, sq, sq < 8 || sq >= 56 ? QUEEN : 0);
            if ((stm ? sq >= 48 : sq < 16) && !(occupied & 1ull << sq + dir)) {
                list[count++] = Move(sq - dir, sq + dir, 0);
            }
        }
        uint64_t pawn_attackable = colors[!stm] | (ep_square ? 1ull << ep_square : 0);
        for (uint64_t bb = e(stm ? s(own_pawns) : n(own_pawns)) & pawn_attackable; bb; bb &= bb-1) {
            int sq = countr_zero(bb);
            list[count++] = Move(sq - dir - 1, sq, sq < 8 || sq >= 56 ? QUEEN : 0);
        }
        for (uint64_t bb = w(stm ? s(own_pawns) : n(own_pawns)) & pawn_attackable; bb; bb &= bb-1) {
            int sq = countr_zero(bb);
            list[count++] = Move(sq - dir + 1, sq, sq < 8 || sq >= 56 ? QUEEN : 0);
        }
        int end = count;
        for (int i = 0; i < end; i++) {
            if (list[i].promo) {
                list[count] = list[i];
                list[count++].promo = KNIGHT;
            }
            if (list[i].to == king_sq[!stm] || list[i].to == castle1 || list[i].to == castle2) {
                return 0;
            }
        }

        for (uint64_t bb = colors[stm] & ~piece[PAWN-1]; bb; bb &= bb - 1) {
            int sq = countr_zero(bb);
            int piece = piece_on(sq);
            uint64_t attacks = 0;
            uint64_t tmp;

            switch (piece) {
                case KING:
                    if (sq == (stm ? E8 : E1)) {
                        if (castle_rights[stm] & SHORT_CASTLE &&
                                occupied & (0b11 << (stm ? 61 : 5))) {
                            list[count++] = Move(sq, sq + 2, 0);
                        }
                        if (castle_rights[stm] & LONG_CASTLE &&
                                occupied & (0b111 << (stm ? 57 : 1))) {
                            list[count++] = Move(sq, sq - 2, 0);
                        }
                    }
                    attacks = n(e(1ull << sq)) | n(1ull << sq) | n(w(1ull << sq))
                        | e(1ull << sq) | w(1ull << sq)
                        | s(e(1ull << sq)) | s(1ull << sq) | s(w(1ull << sq));
                    break;
                case KNIGHT:
                    attacks = n(n(e(1ull << sq))) | n(n(w(1ull << sq)))
                        | n(e(e(1ull << sq))) | s(e(e(1ull << sq)))
                        | n(w(w(1ull << sq))) | s(w(w(1ull << sq)))
                        | s(s(e(1ull << sq))) | s(s(w(1ull << sq)));
                    break;
                case ROOK:
                    tmp = n(1ull << sq);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = s(1ull << sq);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = w(1ull << sq);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = e(1ull << sq);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    attacks |= tmp;
                    break;
                case QUEEN:
                    tmp = n(1ull << sq);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    tmp |= n(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = s(1ull << sq);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    tmp |= s(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = w(1ull << sq);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    tmp |= w(tmp & ~occupied);
                    attacks |= tmp;
                    tmp = e(1ull << sq);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    tmp |= e(tmp & ~occupied);
                    attacks |= tmp;
                    // fall-through
                case BISHOP:
                    tmp = n(e(1ull << sq));
                    tmp |= n(e(tmp & ~occupied));
                    tmp |= n(e(tmp & ~occupied));
                    tmp |= n(e(tmp & ~occupied));
                    tmp |= n(e(tmp & ~occupied));
                    tmp |= n(e(tmp & ~occupied));
                    tmp |= n(e(tmp & ~occupied));
                    attacks |= tmp;
                    tmp = s(e(1ull << sq));
                    tmp |= s(e(tmp & ~occupied));
                    tmp |= s(e(tmp & ~occupied));
                    tmp |= s(e(tmp & ~occupied));
                    tmp |= s(e(tmp & ~occupied));
                    tmp |= s(e(tmp & ~occupied));
                    tmp |= s(e(tmp & ~occupied));
                    attacks |= tmp;
                    tmp = n(w(1ull << sq));
                    tmp |= n(w(tmp & ~occupied));
                    tmp |= n(w(tmp & ~occupied));
                    tmp |= n(w(tmp & ~occupied));
                    tmp |= n(w(tmp & ~occupied));
                    tmp |= n(w(tmp & ~occupied));
                    tmp |= n(w(tmp & ~occupied));
                    attacks |= tmp;
                    tmp = s(w(1ull << sq));
                    tmp |= s(w(tmp & ~occupied));
                    tmp |= s(w(tmp & ~occupied));
                    tmp |= s(w(tmp & ~occupied));
                    tmp |= s(w(tmp & ~occupied));
                    tmp |= s(w(tmp & ~occupied));
                    tmp |= s(w(tmp & ~occupied));
                    attacks |= tmp;
                    break;
            }

            attacks &= ~colors[stm];
            for (; attacks; attacks &= attacks - 1) {
                list[count++] = Move(sq, countr_zero(attacks), 0);
            }
        }
        return 1;
    }

    void update_pawn_eval() {
        eg_pawn_eval = 0;
        mg_pawn_eval = 0;
        for (int file = 1; file < 9; file++) {
            if (pawn_counts[0][file]) {
                mg_pawn_eval -= (pawn_counts[0][file] - 1) * DOUBLED_MG[file-1];
                eg_pawn_eval -= (pawn_counts[0][file] - 1) * DOUBLED_EG[file-1];
            }
            if (pawn_counts[1][file]) {
                mg_pawn_eval += (pawn_counts[1][file] - 1) * DOUBLED_MG[file-1];
                eg_pawn_eval += (pawn_counts[1][file] - 1) * DOUBLED_EG[file-1];
            }
            if (!pawn_counts[0][file-1] && !pawn_counts[0][file+1]) {
                mg_pawn_eval -= ISOLATED_PAWN_MG * pawn_counts[0][file];
                eg_pawn_eval -= ISOLATED_PAWN_EG * pawn_counts[0][file];
            }
            if (!pawn_counts[1][file-1] && !pawn_counts[1][file+1]) {
                mg_pawn_eval += ISOLATED_PAWN_MG * pawn_counts[1][file];
                eg_pawn_eval += ISOLATED_PAWN_EG * pawn_counts[1][file];
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
    }

    int eval() {
        if (pawn_eval_dirty) {
            update_pawn_eval();
            pawn_eval_dirty = 0;
        }
        int bishop_pair = (bishops[0] >= 2) - (bishops[1] >= 2);
        int mg = mg_eval + mg_pawn_eval +
            BISHOP_PAIR_MG * bishop_pair +
            (stm == WHITE ? TEMPO_MG : -TEMPO_MG);
        int eg = eg_eval + eg_pawn_eval +
            BISHOP_PAIR_EG * bishop_pair +
            (stm == WHITE ? TEMPO_EG : -TEMPO_EG);
        int value = (mg * phase + eg * (24 - phase)) / 24;
        return stm == WHITE ? value : -value;
    }
} ROOT;

uint64_t PREHISTORY[256];
int PREHISTORY_LENGTH = 0;
