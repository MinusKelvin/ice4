#define MAX_HIST 16384
#define CORR_HIST_SIZE 16384
#define CORR_HIST_UNIT 228
#define CORR_HIST_DIV 456
#define CORR_HIST_MAX 72

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

atomic_bool ABORT;
mutex MUTEX;
int FINISHED_DEPTH_AND_SCORE;
Move BEST_MOVE;

typedef int16_t HTable[23][SQUARE_SPAN];

struct Searcher {
    uint64_t nodes;
    double hard_limit;
    double soft_limit;
    int16_t evals[256];
    int64_t corr_hist[2][CORR_HIST_SIZE];
    HTable history[23];
    HTable conthist[14][SQUARE_SPAN];
    HTable *conthist_stack[256];
    uint64_t rep_list[256];
    int mobilities[256];

    int negamax(Board &board, Move &bestmv, int alpha, int beta, int depth, int ply) {
        if (depth < 0) {
            depth = 0;
        }

        Move scratch, hashmv{};
        Move moves[256];
        int score[256];
        int mvcount;

        int pv = beta > alpha+1;

        auto& slot = TT[board.zobrist % TT_SIZE];
        uint16_t upper_key = board.zobrist / TT_SIZE;
        TtData tt = slot.load({});
        int tt_good = upper_key == tt.key;
        if (tt_good) {
            if (depth || board.board[tt.mv.to]) {
                hashmv = tt.mv;
            }
            if (depth <= tt.depth && (
                depth*pv <= 1 && tt.bound == BOUND_EXACT ||
                !pv && tt.bound == BOUND_LOWER && tt.eval >= beta ||
                !pv && tt.bound == BOUND_UPPER && tt.eval <= alpha
            )) {
                bestmv = tt.mv;
                return tt.eval;
            }
        } else if (depth > 3) {
            // Internal Iterative Reductions: 6 bytes (v4)
            // 8.0+0.08: 36.52 +- 3.00    6.09 elo/byte
            // 60.0+0.6: 40.34 +- 2.64    6.72 elo/byte
            depth -= 2;
        }

        board.movegen(moves, mvcount, depth, mobilities[ply+1]);

        evals[ply] = board.eval(mobilities[ply+1] - mobilities[ply] + TEMPO)
            + corr_hist[board.stm != WHITE][board.pawn_hash % CORR_HIST_SIZE] / 178
            + corr_hist[board.stm != WHITE][board.material_hash % CORR_HIST_SIZE] / 198
            + (*conthist_stack[ply+1])[0][0] / 102
            + (*conthist_stack[ply])[1][0] / 200;
        int eval = tt_good && tt.eval < 20000 && tt.eval > -20000 ? tt.eval : evals[ply];
        // Improving (only used for LMP): 30 bytes (98fcc8a vs b5fdb00)
        // 8.0+0.08: 28.55 +- 5.11 (3220 - 2400 - 4380) 0.95 elo/byte
        // 60.0+0.6: 29.46 +- 4.55 (2656 - 1810 - 5534) 0.98 elo/byte
        int improving = ply > 1 && evals[ply] > evals[ply-2];

        // Reverse Futility Pruning: 16 bytes (bdf2034 vs 98a56ea)
        // 8.0+0.08: 69.60 +- 5.41 (4085 - 2108 - 3807) 4.35 elo/byte
        // 60.0+0.6: 39.18 +- 4.81 (3060 - 1937 - 5003) 2.45 elo/byte
        if (!pv && !board.check && depth && depth < 8 && eval >= beta + 43 * depth) {
            return eval;
        }

        // Null Move Pruning: 51 bytes (fef0130 vs 98a56ea)
        // 8.0+0.08: 123.85 +- 5.69 (4993 - 1572 - 3435) 2.43 elo/byte
        // 60.0+0.6: 184.01 +- 5.62 (5567 - 716 - 3717) 3.61 elo/byte
        if (!pv && !board.check && eval >= beta && beta > -20000 && depth > 2) {
            Board mkmove = board;
            mkmove.zobrist ^= ZOBRIST[EMPTY][0];
            mkmove.stm ^= INVALID;
            mkmove.ep_square = 0;

            conthist_stack[ply + 2] = &conthist[0][0];

            int reduction = (eval - beta + depth * 27 + 438) / 107;

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - reduction, ply + 1);
            if (v >= beta) {
                return v;
            }
        }

        if (!pv && !board.check && depth && depth < 6 && eval <= alpha - 65 * depth - 195) {
            int v = negamax(board, scratch, alpha, beta, 0, ply);
            if (v <= alpha) {
                return v;
            }
        }

        // Internal Iterative Deepening: 16 bytes (v4)
        // 8.0+0.08: -7.38 +- 2.97    -0.46 elo/byte
        // 60.0+0.6:  9.13 +- 2.63     0.57 elo/byte
        if (depth > 3 && pv && (!tt_good || tt.bound != BOUND_EXACT)) {
            negamax(board, hashmv, alpha, beta, depth - 7, ply);
        }

        for (int j = 0; j < mvcount; j++) {
            if (hashmv.from == moves[j].from && hashmv.to == moves[j].to) {
                score[j] = 1e7;
            } else if (board.board[moves[j].to]) {
                // MVV-LVA capture ordering: 3 bytes (78a3963 vs 35f9b66)
                // 8.0+0.08: 289.03 +- 7.40 (7378 - 563 - 2059) 96.34 elo/byte
                // 60.0+0.6: 237.53 +- 6.10 (6384 - 445 - 3171) 79.18 elo/byte
                score[j] = history[board.board[moves[j].to]][board.board[moves[j].from]][moves[j].to]
                    + (board.board[moves[j].to] & 7) * 1e5;
            } else {
                // Plain history: 28 bytes (676e7fa vs 4cabdf1)
                // 8.0+0.08: 51.98 +- 5.13 (3566 - 2081 - 4353) 1.86 elo/byte
                // 60.0+0.6: 52.37 +- 4.62 (3057 - 1561 - 5382) 1.87 elo/byte
                score[j] = history[board.board[moves[j].to]][board.board[moves[j].from]][moves[j].to]
                    // Continuation histories: 87 bytes (af63703 vs 4cabdf1)
                    // 8.0+0.08: 22.93 +- 5.09 (3124 - 2465 - 4411) 0.26 elo/byte
                    // 60.0+0.6: 46.52 +- 4.57 (2930 - 1599 - 5471) 0.53 elo/byte
                    // Countermove history: 21 bytes (42a57f7 vs 4cabdf1)
                    // 8.0+0.08: 17.98 +- 5.12 (3084 - 2567 - 4349) 0.86 elo/byte
                    // 60.0+0.6: 21.64 +- 4.51 (2508 - 1886 - 5606) 1.03 elo/byte
                    + 2 * (*conthist_stack[ply + 1])[board.board[moves[j].from]][moves[j].to]
                    // Followup history: 22 bytes (ae6f9fa vs 4cabdf1)
                    // 8.0+0.08: 9.07 +- 5.06 (2893 - 2632 - 4475) 0.41 elo/byte
                    // 60.0+0.6: 13.42 +- 4.52 (2396 - 2010 - 5594) 0.61 elo/byte
                    + 2.4 * (*conthist_stack[ply])[board.board[moves[j].from]][moves[j].to];
            }
        }

        rep_list[ply] = board.zobrist;

        int best = depth ? LOST + ply : eval;
        if (best >= beta) {
            return best;
        }

        int quiets_to_check = pv ? -1 : (depth*depth + 10) >> (!improving + 1);

        int raised_alpha = 0;
        int legals = 0;
        for (int i = 0; i < mvcount; i++) {
            int best_so_far = i;
            for (int j = i+1; j < mvcount; j++) {
                if (score[j] > score[best_so_far]) {
                    best_so_far = j;
                }
            }
            swap(moves[i], moves[best_so_far]);
            swap(score[i], score[best_so_far]);

            int victim = board.board[moves[i].to] & 7;

            // Pawn Protected Pruning: 61 bytes (v4)
            // 8.0+0.08: 34.43 +- 3.02     0.56 elo/byte
            // 60.0+0.6: 22.55 +- 2.66     0.37 elo/byte
            int pawn_attacked =
                board.board[moves[i].to + (board.stm & WHITE ? 11 : -11)] == ((board.stm ^ INVALID) | PAWN) ||
                board.board[moves[i].to + (board.stm & WHITE ? 9 : -9)] == ((board.stm ^ INVALID) | PAWN);
            if (ply && pawn_attacked && (board.board[moves[i].from] & 7) > victim + depth / 2) {
                continue;
            }

            // Late Move Pruning (incl. improving): 66 bytes (ee0073a vs b5fdb00)
            // 8.0+0.08: 101.80 +- 5.40 (4464 - 1615 - 3921) 1.54 elo/byte
            // 60.0+0.6: 97.13 +- 4.79 (3843 - 1118 - 5039) 1.47 elo/byte
            if (!(quiets_to_check -= !victim)) {
                break;
            }

            // Delta Pruning: 37 bytes (v4)
            // 8.0+0.08: 25.73 +- 2.98     0.70 elo/byte
            // 60.0+0.6: 22.45 +- 2.62     0.61 elo/byte
            if (!depth && eval + DELTAS[victim] <= alpha) {
                continue;
            }

            Board mkmove = board;
            if (mkmove.make_move(moves[i])) {
                continue;
            }

            conthist_stack[ply + 2] = &conthist[board.board[moves[i].from] - WHITE_PAWN][moves[i].to];
            if (!(++nodes & 0xFFF) && (ABORT || now() > hard_limit)) {
                throw 0;
            }

            int is_rep = 0;
            for (int i = ply-1; depth && !is_rep && i >= 0; i -= 2) {
                is_rep |= rep_list[i] == mkmove.zobrist;
            }
            for (int i = 0; depth && !is_rep && i < PREHISTORY_LENGTH; i++) {
                is_rep |= PREHISTORY[i] == mkmove.zobrist;
            }

            int v;
            int next_depth = depth - 1 + mkmove.check;

            if (is_rep) {
                v = 0;
            } else if (legals) {
                // All reductions: 41 bytes (cedac94 vs b915a59)
                // 8.0+0.08: 184.70 +- 6.16 (5965 - 1099 - 2936) 4.50 elo/byte
                // 60.0+0.6: 213.11 +- 6.04 (6132 - 667 - 3201) 5.20 elo/byte
                // Base LMR: 10 bytes (v4)
                // 8.0+0.08: 80.97 +- 5.10     8.10 elo/byte
                // 60.0+0.6: 83.09 +- 4.65     8.31 elo/byte
                int reduction = LOG[legals] * LOG[max(depth, 0)] * 0.65 + 0.33;
                reduction += hashmv.from && board.board[hashmv.to];
                // History reduction: 9 bytes (v4)
                // 8.0+0.08: 26.28 +- 2.98     2.92 elo/byte
                // 60.0+0.6: 37.09 +- 2.65     4.12 elo/byte
                reduction -= score[i] / 3842;
                if (reduction < 0 || victim) {
                    reduction = 0;
                }

                reduction += victim && score[i] - victim * 1e5 < -5000;

                v = -negamax(mkmove, scratch, -alpha-1, -alpha, next_depth - reduction, ply + 1);
                if (v > alpha && reduction) {
                    // reduced search failed high, re-search at full depth
                    v = -negamax(mkmove, scratch, -alpha-1, -alpha, next_depth, ply + 1);
                }
                if (v > alpha && pv) {
                    // at pv nodes, we need to re-search with full window when move raises alpha
                    // at non-pv nodes, this would be equivalent to the previous search, so skip it
                    v = -negamax(mkmove, scratch, -beta, -alpha, next_depth, ply + 1);
                }
            } else {
                // first legal move is always searched with full window
                v = -negamax(mkmove, scratch, -beta, -alpha, next_depth, ply + 1);
            }
            legals++;
            if (v > best) {
                best = v;
                bestmv = moves[i];
            }
            if (v > alpha) {
                alpha = v;
                raised_alpha = 1;
            }
            if (v >= beta) {
                int bonus = 5.6 * depth * depth;
                bonus <<= ((eval <= alpha) + (eval <= alpha - 42));
                int16_t *hist;
                for (int j = 0; j < i; j++) {
                    if (victim && !board.board[moves[j].to]) {
                        continue;
                    }
                    hist = &history[board.board[moves[j].to]][board.board[moves[j].from]][moves[j].to];
                    *hist -= bonus + bonus * *hist / MAX_HIST;
                    if (!board.board[moves[j].to]) {
                        hist = &(*conthist_stack[ply + 1])[board.board[moves[j].from]][moves[j].to];
                        *hist -= bonus + bonus * *hist / MAX_HIST;
                        hist = &(*conthist_stack[ply])[board.board[moves[j].from]][moves[j].to];
                        *hist -= bonus + bonus * *hist / MAX_HIST;
                    }
                }
                hist = &history[board.board[moves[i].to]][board.board[moves[i].from]][moves[i].to];
                *hist += bonus - bonus * *hist / MAX_HIST;
                if (!victim) {
                    hist = &(*conthist_stack[ply + 1])[board.board[moves[i].from]][moves[i].to];
                    *hist += bonus - bonus * *hist / MAX_HIST;
                    hist = &(*conthist_stack[ply])[board.board[moves[i].from]][moves[i].to];
                    *hist += bonus - bonus * *hist / MAX_HIST;
                }
                break;
            }
        }

        if (depth && legals == 0 && !board.check) {
            return 0;
        }

        if ((depth || best != eval) && best > LOST + ply) {
            tt.key = upper_key;
            tt.eval = best;
            tt.depth = depth;
            tt.bound =
                best >= beta ? BOUND_LOWER :
                raised_alpha ? BOUND_EXACT : BOUND_UPPER;
            if (!tt_good || tt.bound != BOUND_UPPER) {
                tt.mv = bestmv;
            }
            slot.store(tt, {});
            if (!board.board[bestmv.to] && (
                tt.bound == BOUND_UPPER && best < evals[ply] ||
                tt.bound == BOUND_LOWER && best > evals[ply]
            )) {
                double weight = min(depth * depth + 2, 93) / 591.0;
                corr_hist[board.stm != WHITE][board.pawn_hash % CORR_HIST_SIZE] =
                    corr_hist[board.stm != WHITE][board.pawn_hash % CORR_HIST_SIZE] * (1 - weight) +
                    clamp(best - evals[ply], -CORR_HIST_MAX, CORR_HIST_MAX) * CORR_HIST_UNIT * weight;
                corr_hist[board.stm != WHITE][board.material_hash % CORR_HIST_SIZE] =
                    corr_hist[board.stm != WHITE][board.material_hash % CORR_HIST_SIZE] * (1 - weight) +
                    clamp(best - evals[ply], -CORR_HIST_MAX, CORR_HIST_MAX) * CORR_HIST_UNIT * weight;
                (*conthist_stack[ply+1])[0][0] =
                    (*conthist_stack[ply+1])[0][0] * (1 - weight) +
                    clamp(best - evals[ply], -CORR_HIST_MAX, CORR_HIST_MAX) * CORR_HIST_UNIT * weight;
                (*conthist_stack[ply])[1][0] =
                    (*conthist_stack[ply])[1][0] * (1 - weight) +
                    clamp(best - evals[ply], -CORR_HIST_MAX, CORR_HIST_MAX) * CORR_HIST_UNIT * weight;
            }
        }

        return best;
    }

#ifdef OPENBENCH
    void iterative_deepening(int time_alotment, int max_depth=200) {
    #define MAX_DEPTH max_depth
#else
    void iterative_deepening(int time_alotment) {
    #define MAX_DEPTH 200
#endif
        conthist_stack[0] = &conthist[0][1];
        conthist_stack[1] = &conthist[0][1];
        hard_limit = now() + time_alotment * 0.0004;
        soft_limit = now() + time_alotment * 0.000054;
        Move mv;
        int v = 0;
        try {
            for (int depth = 1; depth <= MAX_DEPTH; depth++) {
                // Aspiration windows: 23 bytes (v4)
                // 8.0+0.08: 27.76 +- 2.98    1.21 elo/byte
                // 60.0+0.6: 18.75 +- 2.63    0.82 elo/byte
                int delta = 9;
                int lower = v;
                int upper = v;
                while (v <= lower || v >= upper) {
                    lower = lower > v ? v : lower;
                    upper = upper < v ? v : upper;
                    v = negamax(ROOT, mv, lower -= delta, upper += delta, depth, 0);
                    delta *= 1.8;
                    lock_guard lock(MUTEX);
                    if (v > lower && FINISHED_DEPTH_AND_SCORE < (depth << 20) + v) {
                        FINISHED_DEPTH_AND_SCORE = (depth << 20) + v;
                        BEST_MOVE = mv;
                        printf("info depth %d score cp %d pv ", depth, v);
                        mv.put_with_newline();
                        if (now() > soft_limit) {
                            return;
                        }
                    }
                }
            }
        } catch (...) {}
    }
};
