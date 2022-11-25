#define MAX_HIST 4096

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

struct Searcher {
    uint64_t nodes;
    double abort_time;
    int16_t history[2][7][SQUARE_SPAN];
    uint64_t rep_list[256];
    Move killers[256][2];

    int negamax(Board &board, Move &bestmv, int16_t alpha, int16_t beta, int16_t depth, int ply) {
        Move scratch, hashmv;

        int pv = beta > alpha+1;

        int static_eval = board.eval();

        if (!pv && depth > 0 && depth < 4 && static_eval >= beta + 75 * depth) {
            return static_eval;
        }

        if (!pv && static_eval >= beta && depth > 1) {
            Board mkmove = board;
            mkmove.null_move();

            int reduction = (static_eval - beta) / 128 + depth / 3 + 2;

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - reduction, ply + 1);
            if (v >= beta) {
                return v;
            }
        }

        Move moves[256];
        int score[256];
        int mvcount;
        if (!board.movegen(moves, mvcount, depth > 0)) {
            return WON;
        }

        TtEntry& tt = TT[board.zobrist % TT.size()];

        if (tt.hash == board.zobrist) {
            hashmv = tt.mv;
            if (depth <= tt.depth && (
                tt.bound == BOUND_EXACT ||
                tt.bound == BOUND_LOWER && tt.eval >= beta ||
                tt.bound == BOUND_UPPER && tt.eval <= alpha
            )) {
                bestmv = tt.mv;
                return tt.eval;
            }
        }

        if (depth >= 3 && pv && (
            tt.hash != board.zobrist || tt.depth + 2 < depth || tt.bound != BOUND_EXACT
        )) {
            negamax(board, hashmv, alpha, beta, depth - 2, ply);
        }

        rep_list[ply] = board.zobrist;

        for (int i = 0; i < mvcount; i++) {
            int piece = board.board[moves[i].from] & 7;
            if (hashmv == moves[i]) {
                score[i] = 99999;
            } else if (board.board[moves[i].to]) {
                score[i] = (board.board[moves[i].to] & 7) * 8 - piece + 10000;
            } else if (moves[i] == killers[ply][0] || moves[i] == killers[ply][1]) {
                score[i] = 9000;
            } else {
                score[i] = history[board.stm == BLACK][piece][moves[i].to-A1];
            }
        }

        int raised_alpha = 0;

        int16_t best = depth > 0 ? LOST + ply : static_eval;
        if (best >= beta) return best;

        int quiets_to_check_table[] = { 0, 13, 14, 23 };
        int quiets_to_check = depth > 0 && depth < 4 ? quiets_to_check_table[depth] : 99;

        int legals = 0;
        for (int i = 0; i < mvcount; i++) {
            int best_so_far = i;
            for (int j = i+1; j < mvcount; j++) {
                if (score[j] > score[best_so_far]) {
                    best_so_far = j;
                }
            }
            Move tmp1 = moves[i];
            int tmp2 = score[i];
            moves[i] = moves[best_so_far];
            score[i] = score[best_so_far];
            moves[best_so_far] = tmp1;
            score[best_so_far] = tmp2;

            if (!(quiets_to_check -= !board.board[moves[i].to])) break;

            Board mkmove = board;
            mkmove.make_move(moves[i]);
            if (!(++nodes & 0xFFF) && now() > abort_time) {
                throw 0;
            }

            int is_rep = 0;
            for (int i = ply-1; !is_rep && i >= 0; i -= 2) {
                if (rep_list[i] == mkmove.zobrist) {
                    is_rep = 1;
                }
            }
            for (int i = 0; !is_rep && i < PREHISTORY_LENGTH; i++) {
                if (PREHISTORY[i] == mkmove.zobrist) {
                    is_rep = 1;
                }
            }

            Move scratch;
            int16_t v;

            if (is_rep) {
                v = 0;
            } else if (legals) {
                int reduction = board.board[moves[i].to] ? 0 : legals / 8;
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
                v = -negamax(mkmove, scratch, -beta, -alpha, depth - 1, ply + 1);
            }
            if (v == LOST) {
                moves[i].from = 0;
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
                if (!board.board[moves[i].to]) {
                    for (int j = 0; j < i; j++) {
                        if (board.board[moves[j].to]) continue;
                        int16_t& hist = history[board.stm == BLACK][board.board[moves[j].from] & 7][moves[j].to-A1];
                        int change = depth * 16;
                        hist -= change + change * hist / MAX_HIST;
                    }
                    int16_t& hist = history[board.stm == BLACK][board.board[bestmv.from] & 7][bestmv.to-A1];
                    int change = depth * 16;
                    hist += change - change * hist / MAX_HIST;
                    if (!(killers[ply][0] == bestmv)) {
                        killers[ply][1] = killers[ply][0];
                        killers[ply][0] = bestmv;
                    }
                }
                break;
            }
        }

        if (depth > 0 && legals == 0) {
            Board mkmove = board;
            mkmove.null_move();
            if (mkmove.movegen(moves, mvcount)) {
                return 0;
            }
        }

        if (depth > 0 && best > LOST + ply) {
            tt.hash = board.zobrist;
            tt.mv = bestmv;
            tt.eval = best;
            tt.depth = depth;
            tt.bound =
                best >= beta ? BOUND_LOWER :
                raised_alpha ? BOUND_EXACT : BOUND_UPPER;
        }

        return best;
    }

    void iterative_deepening(double time_alotment, int max_depth=250) {
        memset(history, 0, sizeof(history));
        nodes = 0;
        abort_time = now() + time_alotment * 0.5;
        time_alotment = now() + time_alotment * 0.02;
        Move mv;
        try {
            for (int depth = 1; depth <= max_depth; depth++) {
                int16_t v = negamax(ROOT, mv, LOST, WON, depth, 0);
                printf("info depth %d nodes %ld score cp %d pv ", depth, nodes, v);
                mv.put();
                putchar('\n');
                if (now() > time_alotment) {
                    break;
                }
            }
        } catch (...) {}
        printf("bestmove ");
        mv.put();
        putchar('\n');
    }
};
