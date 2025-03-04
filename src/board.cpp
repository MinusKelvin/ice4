struct Move {
    uint8_t from;
    uint8_t to : 7;
    uint8_t promo : 1;

    void put_with_newline() {
        cout.put(from%10+96)
            .put(from/10+47)
            .put(to%10+96)
            .put(to/10+47)
            .put(promo ? 'q' : ' ')
            << endl;
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
    uint64_t nonpawn_hash[4];

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
            nonpawn_hash[board[square] >> 3] ^= ZOBRIST[board[square]][square];
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
            nonpawn_hash[board[square] >> 3] ^= ZOBRIST[board[square]][square];
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
        zobrist ^= ZOBRIST[EMPTY][0];

        __builtin_prefetch(&TT[zobrist % TT_SIZE]);

        if (attacked(king_sq[stm != WHITE], NSTM)) {
            return 1;
        }

        check = attacked(king_sq[stm == WHITE], stm);
        stm ^= INVALID;
        return 0;

        #undef NSTM
    }

    void movegen(Move list[], int& count, int quiets, int& mobility) {
        // King ring attacks: 30 bytes (v5)
        // 8.0+0.08: 7.47 +- 4.89 [382, 1239, 1891, 1190, 299] 0.25 elo/byte
        // Mobility: 26 bytes (v5)
        // 8.0+0.08: 103.92 +- 5.26 [970, 1765, 1563, 604, 98] 4.00 elo/byte
        uint8_t king_ring[120] = {};
        int attack = 0;
        #define OTHER (stm ^ INVALID)
        count = 0;
        mobility = 0;
        for (int i = 0; i < 8; i++) {
            king_ring[king_sq[stm == WHITE] + RAYS[i]] = 1;
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
                    mobility += MOBILITY[piece];
                    attack += king_ring[sq + dir] * KING_ATTACK_WEIGHT[piece];
                    if (quiets || promo || board[sq + dir + dir + dir] == INVALID) {
                        list[count++] = create_move(sq, sq + dir, promo);
                    }
                    if (board[sq - dir - dir] == INVALID && !board[sq + dir + dir]) {
                        mobility += MOBILITY[piece];
                        attack += king_ring[sq + dir+dir] * KING_ATTACK_WEIGHT[piece];
                        if (quiets) {
                            list[count++] = create_move(sq, sq + dir+dir, promo);
                        }
                    }
                }
                if (ep_square == sq + dir-1 || board[sq + dir-1] & OTHER && ~board[sq + dir-1] & stm) {
                    mobility += MOBILITY[piece];
                    attack += king_ring[sq + dir-1] * KING_ATTACK_WEIGHT[piece];
                    list[count++] = create_move(sq, sq + dir-1, promo);
                }
                if (ep_square == sq + dir+1 || board[sq + dir+1] & OTHER && ~board[sq + dir+1] & stm) {
                    mobility += MOBILITY[piece];
                    attack += king_ring[sq + dir+1] * KING_ATTACK_WEIGHT[piece];
                    list[count++] = create_move(sq, sq + dir+1, promo);
                }
            } else {
                for (int i = STARTS[piece]; i < ENDS[piece]; i++) {
                    int raysq = sq;
                    for (;;) {
                        raysq += RAYS[i];
                        if (board[raysq] & stm) {
                            if (piece == ROOK && (board[raysq] & 7) == ROOK && raysq > sq) {
                                mobility += CONNECTED_ROOKS;
                            }
                            break;
                        }
                        mobility += MOBILITY[piece];
                        attack += king_ring[raysq] * KING_ATTACK_WEIGHT[piece];
                        if (board[raysq] & OTHER) {
                            list[count++] = create_move(sq, raysq, 0);
                            break;
                        } else if (quiets) {
                            list[count++] = create_move(sq, raysq, 0);
                        }

                        // breaks loop for non-sliders
                        //       PAWN   KNIGHT BISHOP ROOK   QUEEN  KING
                        // repr  1      2      3      4      5      6
                        // / 3   0      0      1      1      1      2
                        // - 1   -1     -1     0      0      0      1
                        // bool  true   true   false  false  false  true
                        if (piece / 3 - 1) {
                            break;
                        }
                    }
                }
            }
        }
        mobility += attack * attack / 160;
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
                        + get_data(OWN_KING_PASSER_DIST_INDEX + max(
                            abs(sq / 10 - king_sq[ci] / 10),
                            abs(file - king_sq[ci] % 10)
                        ))
                        + get_data(OPP_KING_PASSER_DIST_INDEX + max(
                            abs(sq / 10 - king_sq[!ci] / 10),
                            abs(file - king_sq[!ci] % 10)
                        ))
                        + (PASSER_RANK + OWN_KING_PASSER_DIST + OPP_KING_PASSER_DIST);
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
Board parse_fen(istream& stream) {
    Board board;
    string token;

    stream >> token;
    int rank = 7;
    int file = 0;
    for (char c : token) {
        int sq = (rank * 10) + file + A1;
        file++;
        switch (c) {
            case 'P':
                board.edit(sq, WHITE | PAWN);
                break;
            case 'N':
                board.edit(sq, WHITE | KNIGHT);
                break;
            case 'B':
                board.edit(sq, WHITE | BISHOP);
                break;
            case 'R':
                board.edit(sq, WHITE | ROOK);
                break;
            case 'Q':
                board.edit(sq, WHITE | QUEEN);
                break;
            case 'K':
                board.edit(sq, WHITE | KING);
                break;
            case 'p':
                board.edit(sq, BLACK | PAWN);
                break;
            case 'n':
                board.edit(sq, BLACK | KNIGHT);
                break;
            case 'b':
                board.edit(sq, BLACK | BISHOP);
                break;
            case 'r':
                board.edit(sq, BLACK | ROOK);
                break;
            case 'q':
                board.edit(sq, BLACK | QUEEN);
                break;
            case 'k':
                board.edit(sq, BLACK | KING);
                break;
            case '/':
                file = 0;
                rank--;
                break;
            default:
                file += c - '1';
                for (int i = 0; i < c - '0'; i++) {
                    board.edit(sq+i, 0);
                }
                break;
        }
    }

    stream >> token;
    if (token == "b") {
        board.stm = BLACK;
        board.zobrist ^= ZOBRIST[EMPTY][0];
    }

    stream >> token;
    board.zobrist ^= ZOBRIST[EMPTY][board.castle_rights];
    board.castle_rights = WHITE_SHORT_CASTLE | WHITE_LONG_CASTLE | BLACK_SHORT_CASTLE | BLACK_LONG_CASTLE;
    for (char c : token) {
        switch (c) {
            case 'K':
                board.castle_rights ^= WHITE_SHORT_CASTLE;
                break;
            case 'Q':
                board.castle_rights ^= WHITE_LONG_CASTLE;
                break;
            case 'k':
                board.castle_rights ^= BLACK_SHORT_CASTLE;
                break;
            case 'q':
                board.castle_rights ^= BLACK_LONG_CASTLE;
                break;
        }
    }
    board.zobrist ^= ZOBRIST[EMPTY][board.castle_rights];

    stream >> token;
    if (token != "-") {
        board.zobrist ^= ZOBRIST[EMPTY][board.ep_square];
        board.ep_square = (token[1] - '1') * 10 + token[0] - 'a' + A1;
        board.zobrist ^= ZOBRIST[EMPTY][board.ep_square];
    }

    stream >> token >> token; // ignore halfmove clock and fullmove number

    return board;
}
#endif
