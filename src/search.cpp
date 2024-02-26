#define MAX_HIST 4096

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

atomic_bool ABORT;
mutex MUTEX;
int FINISHED_DEPTH;
Move BEST_MOVE(0);

typedef int16_t HTable[16][SQUARE_SPAN];

struct Searcher {
    uint64_t nodes;
    double abort_time;
    double start;
    int16_t evals[256];
    HTable history;
    HTable conthist[14][SQUARE_SPAN];
    HTable *conthist_stack[256];
    uint64_t rep_list[256];
    uint64_t root_nodes[SQUARE_SPAN][SQUARE_SPAN];
    int mobilities[256];

    int negamax(Board &board, Move &bestmv, int alpha, int beta, int depth, int ply) {
        Move scratch, hashmv(0);
        Move moves[256];
        int score[256];
        int mvcount, scratch_int;

        int pv = beta > alpha+1;

        auto& slot = TT[board.zobrist % TT_SIZE_EXPR];
        uint16_t upper_key = board.zobrist / TT_SIZE_EXPR;
        TtData tt = slot.load(memory_order_relaxed);
        int tt_good = upper_key == tt.key;
        if (tt_good) {
            if (depth > 0 || board.board[tt.mv.to]) {
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
            // Internal Iterative Reductions: 8 bytes (524f0e8 vs b5fdb00)
            // 8.0+0.08: 0.66 +- 5.07 (2790 - 2771 - 4439) 0.08 elo/byte
            // 60.0+0.6: 22.30 +- 4.52 (2530 - 1889 - 5581) 2.79 elo/byte
            depth--;
        }

        board.movegen(moves, mvcount, depth > 0, mobilities[ply+1]);

        evals[ply] = board.eval(mobilities[ply+1] - mobilities[ply] + TEMPO);
        int eval = tt_good && tt.eval < 20000 && tt.eval > -20000 ? tt.eval : evals[ply];
        // Improving (only used for LMP): 30 bytes (98fcc8a vs b5fdb00)
        // 8.0+0.08: 28.55 +- 5.11 (3220 - 2400 - 4380) 0.95 elo/byte
        // 60.0+0.6: 29.46 +- 4.55 (2656 - 1810 - 5534) 0.98 elo/byte
        int improving = ply > 1 && evals[ply] > evals[ply-2];

        // Reverse Futility Pruning: 16 bytes (bdf2034 vs 98a56ea)
        // 8.0+0.08: 69.60 +- 5.41 (4085 - 2108 - 3807) 4.35 elo/byte
        // 60.0+0.6: 39.18 +- 4.81 (3060 - 1937 - 5003) 2.45 elo/byte
        if (!pv && !board.check && depth > 0 && depth < 7 && eval >= beta + 77 * depth) {
            return eval;
        }

        if (!pv && depth == 1 && eval <= alpha - 213) {
            return negamax(board, bestmv, alpha, beta, 0, ply);
        }

        // Null Move Pruning: 51 bytes (fef0130 vs 98a56ea)
        // 8.0+0.08: 123.85 +- 5.69 (4993 - 1572 - 3435) 2.43 elo/byte
        // 60.0+0.6: 184.01 +- 5.62 (5567 - 716 - 3717) 3.61 elo/byte
        if (!pv && !board.check && eval >= beta && beta > -20000 && depth > 1) {
            Board mkmove = board;
            mkmove.null_move();
            conthist_stack[ply + 2] = &conthist[0][0];

            int reduction = (eval - beta) / 76 + depth * 0.38 + 3;

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - reduction, ply + 1);
            if (v >= beta) {
                return v;
            }
        }

        // Internal Iterative Deepening: 24 bytes (bd674e0 vs 98a56ea)
        // 8.0+0.08: 67.08 +- 5.38 (4027 - 2120 - 3853) 2.80 elo/byte
        // 60.0+0.6: 94.47 +- 4.95 (3952 - 1298 - 4750) 3.94 elo/byte
        if (depth >= 2 && pv && (!tt_good || tt.bound != BOUND_EXACT)) {
            negamax(board, hashmv, alpha, beta, depth - 5, ply);
        }

        for (int j = 0; j < mvcount; j++) {
            if (hashmv.from == moves[j].from && hashmv.to == moves[j].to) {
                score[j] = 1e6;
            } else if (board.board[moves[j].to]) {
                // MVV-LVA capture ordering: 3 bytes (78a3963 vs 35f9b66)
                // 8.0+0.08: 289.03 +- 7.40 (7378 - 563 - 2059) 96.34 elo/byte
                // 60.0+0.6: 237.53 +- 6.10 (6384 - 445 - 3171) 79.18 elo/byte
                score[j] = (board.board[moves[j].to] & 7) * 8
                    - (board.board[moves[j].from] & 7)
                    + 1e5;
            } else {
                // Plain history: 28 bytes (676e7fa vs 4cabdf1)
                // 8.0+0.08: 51.98 +- 5.13 (3566 - 2081 - 4353) 1.86 elo/byte
                // 60.0+0.6: 52.37 +- 4.62 (3057 - 1561 - 5382) 1.87 elo/byte
                score[j] = history[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1]
                    // Continuation histories: 87 bytes (af63703 vs 4cabdf1)
                    // 8.0+0.08: 22.93 +- 5.09 (3124 - 2465 - 4411) 0.26 elo/byte
                    // 60.0+0.6: 46.52 +- 4.57 (2930 - 1599 - 5471) 0.53 elo/byte
                    // Countermove history: 21 bytes (42a57f7 vs 4cabdf1)
                    // 8.0+0.08: 17.98 +- 5.12 (3084 - 2567 - 4349) 0.86 elo/byte
                    // 60.0+0.6: 21.64 +- 4.51 (2508 - 1886 - 5606) 1.03 elo/byte
                    + 2 * (*conthist_stack[ply + 1])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1]
                    // Followup history: 22 bytes (ae6f9fa vs 4cabdf1)
                    // 8.0+0.08: 9.07 +- 5.06 (2893 - 2632 - 4475) 0.41 elo/byte
                    // 60.0+0.6: 13.42 +- 4.52 (2396 - 2010 - 5594) 0.61 elo/byte
                    + 3 * (*conthist_stack[ply])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
            }
        }

        rep_list[ply] = board.zobrist;

        int best = depth > 0 ? LOST + ply : eval;
        if (best >= beta) {
            return best;
        }

        int quiets_to_check = pv ? -1 : (depth*depth - depth + 4) >> !improving;

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
            int deltas[] = {814, 139, 344, 403, 649, 867, 0};

            int opp_pawn = (board.stm ^ INVALID) | PAWN;
            int pawn_attacked =
                board.board[moves[i].to + (board.stm & WHITE ? 11 : -11)] == opp_pawn ||
                board.board[moves[i].to + (board.stm & WHITE ? 9 : -9)] == opp_pawn;
            if (ply && pawn_attacked && (board.board[moves[i].from] & 7) > victim + max(0, depth) / 2) {
                continue;
            }

            // Late Move Pruning (incl. improving): 66 bytes (ee0073a vs b5fdb00)
            // 8.0+0.08: 101.80 +- 5.40 (4464 - 1615 - 3921) 1.54 elo/byte
            // 60.0+0.6: 97.13 +- 4.79 (3843 - 1118 - 5039) 1.47 elo/byte
            if (!(quiets_to_check -= !victim)) {
                break;
            }

            // Delta Pruning: 37 bytes (939b3de vs 4cabdf1)
            // 8.0+0.08: 25.30 +- 5.11 (3175 - 2448 - 4377) 0.68 elo/byte
            // 60.0+0.6: 21.67 +- 4.55 (2551 - 1928 - 5521) 0.59 elo/byte
            if (depth <= 0 && eval + deltas[victim] <= alpha) {
                continue;
            }

            Board mkmove = board;
            if (mkmove.make_move(moves[i])) {
                continue;
            }

            conthist_stack[ply + 2] = &conthist[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
            if (!(++nodes & 0xFFF) && (ABORT || now() > abort_time)) {
                throw 0;
            }

            int is_rep = 0;
            for (int i = ply-1; depth > 0 && !is_rep && i >= 0; i -= 2) {
                is_rep |= rep_list[i] == mkmove.zobrist;
            }
            for (int i = 0; depth > 0 && !is_rep && i < PREHISTORY_LENGTH; i++) {
                is_rep |= PREHISTORY[i] == mkmove.zobrist;
            }

            int v;
            int next_depth = depth - 1 + mkmove.check;
            uint64_t old_nodes = nodes;

            if (is_rep) {
                v = 0;
            } else if (legals) {
                // All reductions: 41 bytes (cedac94 vs b915a59)
                // 8.0+0.08: 184.70 +- 6.16 (5965 - 1099 - 2936) 4.50 elo/byte
                // 60.0+0.6: 213.11 +- 6.04 (6132 - 667 - 3201) 5.20 elo/byte
                int reduction = legals * 0.144 + DEPTH_LMR[max(depth, 0)];
                // History Reduction: 6 bytes (bf488d7 vs 0e2f650)
                // 8.0+0.08: 17.60 +- 5.06 (3011 - 2505 - 4484) 2.93 elo/byte
                // 60.0+0.6: 48.01 +- 4.69 (3062 - 1689 - 5249) 8.00 elo/byte
                reduction -= score[i] / 580;
                if (reduction < 0 || victim) {
                    reduction = 0;
                }
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
            if (!ply) {
                root_nodes[moves[i].from-A1][moves[i].to-A1] += nodes - old_nodes;
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
                if (!victim) {
                    int16_t *hist;
                    for (int j = 0; j < i; j++) {
                        if (board.board[moves[j].to]) {
                            continue;
                        }
                        hist = &history[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                        *hist -= depth * depth + depth * depth * *hist / MAX_HIST;
                        hist = &(*conthist_stack[ply + 1])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                        *hist -= depth * depth + depth * depth * *hist / MAX_HIST;
                        hist = &(*conthist_stack[ply])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                        *hist -= depth * depth + depth * depth * *hist / MAX_HIST;
                    }
                    hist = &history[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                    *hist += depth * depth - depth * depth * *hist / MAX_HIST;
                    hist = &(*conthist_stack[ply + 1])[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                    *hist += depth * depth - depth * depth * *hist / MAX_HIST;
                    hist = &(*conthist_stack[ply])[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                    *hist += depth * depth - depth * depth * *hist / MAX_HIST;
                }
                break;
            }
        }

        if (depth > 0 && legals == 0 && !board.check) {
            return 0;
        }

        if ((depth > 0 || best != eval) && best > LOST + ply) {
            tt.key = upper_key;
            tt.eval = best;
            tt.depth = depth > 0 ? depth : 0;
            tt.bound =
                best >= beta ? BOUND_LOWER :
                raised_alpha ? BOUND_EXACT : BOUND_UPPER;
            if (!tt_good || tt.bound != BOUND_UPPER) {
                tt.mv = bestmv;
            }
            slot.store(tt, memory_order_relaxed);
        }

        return best;
    }

    void iterative_deepening(double time_alotment, int max_depth=200) {
        memset(this, 0, sizeof(Searcher));
        nodes = 0;
        conthist_stack[0] = &conthist[0][1];
        conthist_stack[1] = &conthist[0][1];
        start = now();
        abort_time = start + time_alotment * 0.5;
        Move mv;
        int last_score = 0;
        try {
            for (int depth = 1; depth <= max_depth; depth++) {
                int window = 7;
                int v = last_score + window;
                while (v <= last_score - window || v >= last_score + window) {
                    window *= 2;
                    v = negamax(ROOT, mv, last_score - window, last_score + window, depth, 0);
                }
                last_score = v;
#ifdef AVOID_ADJUDICATION
                v = 100;
#endif
                MUTEX.lock();
                if (FINISHED_DEPTH < depth) {
                    double nodes_fraction = (double) root_nodes[mv.from-A1][mv.to-A1] / nodes;
                    BEST_MOVE = mv;
                    printf("info depth %d score cp %d pv ", depth, v);
                    mv.put_with_newline();
                    FINISHED_DEPTH = depth;
                    if (now() - start > time_alotment * 0.05 * (1.5 - nodes_fraction)) {
                        depth = max_depth;
                    }
                }
                MUTEX.unlock();
            }
        } catch (...) {}
    }
};
