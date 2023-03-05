#define MAX_HIST 4096

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

atomic_bool ABORT;
mutex MUTEX;
int FINISHED_DEPTH;
Move BEST_MOVE(0);

typedef int16_t HTable[16][SQUARE_SPAN];

struct Searcher {
    uint64_t nodes;
    double abort_time;
    int16_t evals[256];
    HTable history;
    HTable conthist[14][SQUARE_SPAN];
    HTable *conthist_stack[256];
    uint64_t rep_list[256];

    int negamax(Board &board, Move &bestmv, int16_t alpha, int16_t beta, int16_t depth, int ply) {
        Move scratch, hashmv(0);
        Move moves[256];
        int score[256];
        int mvcount;

        int pv = beta > alpha+1;
        // Check Conditions: 24 bytes (46d9d80 vs 0f4a84d)
        // 8.0+0.08: 14.84 +- 5.12 (3046 - 2619 - 4335) 0.62 elo/byte
        int in_check = 0;

        TtEntry& slot = TT[board.zobrist % TT.size()];
        uint64_t data = slot.data.load(memory_order_relaxed);
        uint64_t hash_xor_data = slot.hash_xor_data.load(memory_order_relaxed);
        int tt_good = (data ^ board.zobrist) == hash_xor_data;
        TtData tt;
        if (tt_good) {
            memcpy(&tt, &data, sizeof(TtData));

            if (depth > 0 || board.board[tt.mv.to]) {
                hashmv = tt.mv;
            }
            if (depth <= tt.depth && (
                tt.bound == BOUND_EXACT ||
                tt.bound == BOUND_LOWER && tt.eval >= beta ||
                tt.bound == BOUND_UPPER && tt.eval <= alpha
            )) {
                bestmv = tt.mv;
                return tt.eval;
            }
        } else if (depth > 5) {
            // Internal Iterative Reductions: 8 bytes (524f0e8 vs b5fdb00)
            // 8.0+0.08: 0.66 +- 5.07 (2790 - 2771 - 4439) 0.08 elo/byte
            // 60.0+0.6: 22.30 +- 4.52 (2530 - 1889 - 5581) 2.79 elo/byte
            depth--;
        }

        evals[ply] = board.eval();
        int eval = tt_good && tt.eval < 20000 && tt.eval > -20000 ? tt.eval : evals[ply];
        // Improving (only used for LMP): 30 bytes (98fcc8a vs b5fdb00)
        // 8.0+0.08: 28.55 +- 5.11 (3220 - 2400 - 4380) 0.95 elo/byte
        int improving = ply > 1 && evals[ply] > evals[ply-2];

        // Reverse Futility Pruning: 16 bytes (bdf2034 vs 98a56ea)
        // 8.0+0.08: 69.60 +- 5.41 (4085 - 2108 - 3807) 4.35 elo/byte
        // 60.0+0.6: 39.18 +- 4.81 (3060 - 1937 - 5003) 2.45 elo/byte
        if (!pv && depth > 0 && depth < 4 && eval >= beta + 75 * depth) {
            return eval;
        }

        // Null Move Pruning: 51 bytes (fef0130 vs 98a56ea)
        // 8.0+0.08: 123.85 +- 5.69 (4993 - 1572 - 3435) 2.43 elo/byte
        // 60.0+0.6: 184.01 +- 5.62 (5567 - 716 - 3717) 3.61 elo/byte
        if (!pv && eval >= beta && beta > -20000 && depth > 1) {
            Board mkmove = board;
            mkmove.null_move();
            conthist_stack[ply] = &conthist[0][0];

            int reduction = (eval - beta) / 128 + depth / 3 + 2;

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - reduction, ply + 1);
            if (v >= beta) {
                return v;
            }
            in_check = v == LOST;
        }

        if (pv && depth > 0) {
            Board mkmove = board;
            mkmove.null_move();
            in_check = !mkmove.movegen(moves, mvcount);
        }

        // Internal Iterative Deepening: 24 bytes (bd674e0 vs 98a56ea)
        // 8.0+0.08: 67.08 +- 5.38 (4027 - 2120 - 3853) 2.80 elo/byte
        // 60.0+0.6: 94.47 +- 4.95 (3952 - 1298 - 4750) 3.94 elo/byte
        if (depth >= 4 && pv && (!tt_good || tt.bound != BOUND_EXACT)) {
            negamax(board, hashmv, alpha, beta, depth - 3, ply);
        }

        rep_list[ply] = board.zobrist;
        moves[0] = hashmv;
        score[0] = 0;
        mvcount = 1;

        int best = depth > 0 ? LOST + ply : eval;
        if (best >= beta) {
            return best;
        }

        int quiets_to_check_table[] = { 0, 7, 8, 17, 49 };
        int quiets_to_check = depth > 0 && depth < 5 && !pv ? quiets_to_check_table[depth] / (1 + !improving) : -1;

        int raised_alpha = 0;
        int legals = 0;
        for (int i = 0; i < mvcount; i++) {
            if (moves[i].from) {
                int best_so_far = i;
                for (int j = i+1; j < mvcount; j++) {
                    if (score[j] > score[best_so_far]) {
                        best_so_far = j;
                    }
                }
                swap(moves[i], moves[best_so_far]);
                swap(score[i], score[best_so_far]);

                int victim = board.board[moves[i].to] & 7;
                int deltas[] = {1350, 210, 390, 440, 680, 1350, 0};

                // Late Move Pruning (incl. improving): 66 bytes (ee0073a vs b5fdb00)
                // 8.0+0.08: 101.80 +- 5.40 (4464 - 1615 - 3921) 1.54 elo/byte
                if (!(quiets_to_check -= !victim)) {
                    break;
                }

                if (depth <= 0 && eval + deltas[victim] <= alpha) {
                    continue;
                }

                Board mkmove = board;
                mkmove.make_move(moves[i]);
                conthist_stack[ply] = &conthist[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
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

                if (is_rep) {
                    v = 0;
                } else if (legals) {
                    // All reductions: 57 bytes (a8e89fa vs 98a56ea)
                    // 8.0+0.08: 181.21 +- 6.27 (6020 - 1231 - 2749) 3.18 elo/byte
                    // 60.0+0.6: 179.68 +- 5.89 (5716 - 961 - 3323) 3.15 elo/byte
                    int reduction = (legals*3 + depth*2) / 32;
                    if (reduction > legals) {
                        reduction = legals;
                    }
                    reduction += legals > 3;
                    reduction -= score[i] / 400;
                    if (reduction < 0 || victim || in_check) {
                        reduction = 0;
                    }
                    v = -negamax(mkmove, scratch, -alpha-1, -alpha, depth - reduction - 1, ply + 1);
                    if (v > alpha && reduction) {
                        // reduced search failed high, re-search at full depth
                        v = -negamax(mkmove, scratch, -alpha-1, -alpha, depth - 1, ply + 1);
                    }
                    if (v > alpha && v < beta) {
                        // at pv nodes, we need to re-search with full window when move raises alpha
                        // at non-pv nodes, this would be equivalent to the previous search, so skip it
                        v = -negamax(mkmove, scratch, -beta, -alpha, depth - 1, ply + 1);
                    }
                } else {
                    // first legal move is always searched with full window
                    v = -negamax(mkmove, scratch, -beta, -alpha, depth - 1 + in_check, ply + 1);
                }
                if (v == LOST) {
                    moves[i].from = 1;
                } else {
                    legals++;
                }
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
                        int change = depth * depth;
                        int16_t *hist;
                        for (int j = 0; j < i; j++) {
                            if (board.board[moves[j].to]) {
                                continue;
                            }
                            hist = &history[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                            *hist -= change + change * *hist / MAX_HIST;
                            if (ply) {
                                hist = &(*conthist_stack[ply - 1])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                                *hist -= change + change * *hist / MAX_HIST;
                            }
                            if (ply > 1) {
                                hist = &(*conthist_stack[ply - 2])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1];
                                *hist -= change + change * *hist / MAX_HIST;
                            }
                        }
                        hist = &history[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                        *hist += change - change * *hist / MAX_HIST;
                        if (ply) {
                            hist = &(*conthist_stack[ply - 1])[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                            *hist += change - change * *hist / MAX_HIST;
                        }
                        if (ply > 1) {
                            hist = &(*conthist_stack[ply - 2])[board.board[moves[i].from] - WHITE_PAWN][moves[i].to-A1];
                            *hist += change - change * *hist / MAX_HIST;
                        }
                    }
                    break;
                }
            }

            // cases that reach this point with i == 0:
            // 1. hashmv does not exist, moves[0] does not exist => movegen
            // 2. hashmv does not exist, moves[0] exists => already did movegen
            // 3. hashmv exists (implies moves[0] exists) => movegen
            if (!i && (!moves[0].from || hashmv.from)) {
                if (!board.movegen(moves, mvcount, depth > 0)) {
                    return WON;
                }
                for (int j = 0; j < mvcount; j++) {
                    if (hashmv == moves[j]) {
                        swap(moves[0], moves[j]);
                        swap(score[0], score[j]);
                    } else if (board.board[moves[j].to]) {
                        // MVV-LVA capture ordering: 3 bytes (78a3963 vs 35f9b66)
                        // 8.0+0.08: 289.03 +- 7.40 (7378 - 563 - 2059) 96.34 elo/byte
                        // 60.0+0.6: 237.53 +- 6.10 (6384 - 445 - 3171) 79.18 elo/byte
                        score[j] = (board.board[moves[j].to] & 7) * 8
                            - (board.board[moves[j].from] & 7)
                            + 20000;
                    } else {
                        // (outdated) History heuristic: 90 bytes (d2a7a0e vs 35f9b66)
                        // 8.0+0.08: 225.18 +- 6.42 (6467 - 763 - 2770) 2.50 elo/byte
                        score[j] = history[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1]
                            + (ply ?
                                (*conthist_stack[ply - 1])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1]
                            : 0)
                            + (ply > 1 ?
                                (*conthist_stack[ply - 2])[board.board[moves[j].from] - WHITE_PAWN][moves[j].to-A1]
                            : 0);
                    }
                }
                // need to step back loop variable in case 1
                i -= !hashmv.from;
            }
        }

        if (depth > 0 && legals == 0) {
            Board mkmove = board;
            mkmove.null_move();
            if (mkmove.movegen(moves, mvcount)) {
                return 0;
            }
        }

        if ((depth > 0 || best != eval) && best > LOST + ply) {
            tt.mv = bestmv;
            tt.eval = best;
            tt.depth = depth > 0 ? depth : 0;
            tt.bound =
                best >= beta ? BOUND_LOWER :
                raised_alpha ? BOUND_EXACT : BOUND_UPPER;
            memcpy(&data, &tt, sizeof(TtData));
            slot.data.store(data, memory_order_relaxed);
            slot.hash_xor_data.store(data ^ board.zobrist, memory_order_relaxed);
        }

        return best;
    }

    void iterative_deepening(double time_alotment, int max_depth=250) {
        memset(history, 0, sizeof(history));
        memset(conthist, 0, sizeof(conthist));
        nodes = 0;
        abort_time = now() + time_alotment * 0.5;
        time_alotment = now() + time_alotment * 0.03;
        Move mv(0);
        try {
            for (int depth = 1; depth <= max_depth; depth++) {
                int v = negamax(ROOT, mv, LOST, WON, depth, 0);
                MUTEX.lock();
                if (FINISHED_DEPTH < depth) {
                    BEST_MOVE = mv;
                    printf("info depth %d score cp %d pv ", depth, v);
                    mv.put();
                    putchar('\n');
                    FINISHED_DEPTH = depth;
                    if (now() > time_alotment) {
                        depth = max_depth;
                    }
                }
                MUTEX.unlock();
            }
        } catch (...) {}
    }
};
