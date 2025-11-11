#define MAX_HIST 16384
#define CORR_HIST_SIZE 16384
#define CORR_HIST_UNIT 280
#define CORR_HIST_DIV 486
#define CORR_HIST_MAX 81

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

atomic_bool ABORT;
mutex MUTEX;
int FINISHED_DEPTH;
Move BEST_MOVE;

typedef int16_t HTable[23][SQUARE_SPAN];

struct Searcher {
    uint64_t nodes;
    double hard_limit;
    double soft_limit;
    uint64_t rep_list[256];
    int mobilities[256];
    int evals[256];
    HTable history[7];
    HTable *conthist_stack[256];
    HTable conthist[14][SQUARE_SPAN];

    int negamax(Board &board, Move &bestmv, int alpha, int beta, int depth, int ply, Move excluded = Move()) {
        if (depth < 0) {
            depth = 0;
        }

        auto& slot = TT[board.zobrist >> (64 - TT_BITS)];
        TtData tt = slot.load({});

        Move scratch;
        Move moves[256];
        int score[256];
        int mvcount;
        int pv = beta > alpha+1;

        tt.key ^= board.zobrist;
        if (!excluded.from && !tt.key) {
            if (!pv && depth <= tt.depth && (
                tt.bound & BOUND_LOWER && tt.score >= beta ||
                tt.bound & BOUND_UPPER && tt.score <= alpha
            )) {
                return tt.score;
            }
        }

        board.movegen(moves, mvcount, depth, mobilities[ply+1]);

        int eval = board.eval(mobilities[ply+1] - mobilities[ply] + TEMPO);
        int improving = ply > 1 && !board.check && eval > evals[ply-2];
        evals[ply] = board.check ? WON : eval;
        rep_list[ply] = board.zobrist;

        if (!excluded.from && !pv && !board.check && depth < 5 && eval > beta + max(0, depth - improving) * 38) {
            return eval;
        }

        if (!excluded.from && !pv && !board.check && eval >= beta && beta > -20000 && depth > 2) {
            Board mkmove = board;
            mkmove.stm ^= INVALID;
            mkmove.zobrist ^= ZOBRIST[EMPTY][mkmove.ep_square];
            mkmove.ep_square = 0;

            conthist_stack[ply + 2] = &conthist[0][0];

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - 5, ply + 1);
            if (v >= beta) {
                return v;
            }
        }

        for (int i = 0; i < mvcount; i++) {
            score[i] =
                !tt.key && tt.mv.from == moves[i].from && tt.mv.to == moves[i].to ? 1e7
                : board.board[moves[i].to] ? board.board[moves[i].to] * 1e5
                    + history[board.board[moves[i].to] & 7][board.board[moves[i].from]][moves[i].to]
                : history[0][board.board[moves[i].from]][moves[i].to]
                    + (*conthist_stack[ply + 1])[board.board[moves[i].from]][moves[i].to]
                    + (*conthist_stack[ply])[board.board[moves[i].from]][moves[i].to];
        }

        int best = depth ? LOST + ply : eval;
        int quiets_to_check = (depth * depth + 3) >> !improving;
        int orig_alpha = alpha;
        int legals = 0;

        if (best >= beta) {
            return best;
        }

        for (int i = 0; i < mvcount; i++) {
            int best_so_far = i;
            for (int j = i+1; j < mvcount; j++) {
                if (score[j] > score[best_so_far]) {
                    best_so_far = j;
                }
            }
            swap(moves[i], moves[best_so_far]);
            swap(score[i], score[best_so_far]);

            if (moves[i].from == excluded.from && moves[i].to == excluded.to) {
                continue;
            }

            int victim = board.board[moves[i].to] & 7;

            Board mkmove = board;
            if (mkmove.make_move(moves[i])) {
                continue;
            }
            conthist_stack[ply + 2] = &conthist[board.board[moves[i].from] - WHITE_PAWN][moves[i].to];

            if (!pv && !victim && !(quiets_to_check -= 1)) {
                break;
            }

            if (!(++nodes & 0xFFF) && (ABORT || now() > hard_limit)) {
                throw 0;
            }

            int is_rep = 0;
            int v;
            int next_depth = depth - 1;

            for (int i = ply-1; depth && !is_rep && i >= 0; i -= 2) {
                is_rep |= rep_list[i] == mkmove.zobrist;
            }
            for (int i = 0; depth && !is_rep && i < PREHISTORY_LENGTH; i++) {
                is_rep |= PREHISTORY[i] == mkmove.zobrist;
            }

            if (is_rep) {
                v = 0;
            } else if (legals) {
                int reduction = LOG[legals] * LOG[depth] * 0.69 - 0.11;

                if (victim) {
                    reduction = 0;
                }

                v = -negamax(mkmove, scratch, -alpha-1, -alpha, next_depth - reduction, ply + 1);
                if (v > alpha && reduction) {
                    v = -negamax(mkmove, scratch, -alpha-1, -alpha, next_depth, ply + 1);
                }
                if (v > alpha && pv) {
                    // at pv nodes, we need to re-search with full window when move raises alpha
                    // at non-pv nodes, this would be equivalent to the previous search, so skip it
                    v = -negamax(mkmove, scratch, -beta, -alpha, next_depth, ply + 1);
                }
            } else {
                if (
                    score[i] == 1e7 &&
                    depth > 7 &&
                    ply &&
                    tt.depth > depth - 5 &&
                    tt.bound != BOUND_UPPER &&
                    tt.score < 20000 && tt.score > -20000
                ) {
                    int s_beta = tt.score - 2 * depth;
                    int score = negamax(board, scratch, s_beta-1, s_beta, depth / 2, ply, moves[i]);
                    if (score < s_beta) {
                        next_depth++;
                    }
                }

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
            }
            if (v >= beta) {
                int bonus = 32 * depth;
                for (int j = 0; j < i; j++) {
                    if (victim && !board.board[moves[j].to]) {
                        continue;
                    }
                    int16_t *hist = &history[board.board[moves[j].to] & 7][board.board[moves[j].from]][moves[j].to];
                    *hist -= bonus + bonus * *hist / MAX_HIST;
                    if (!board.board[moves[j].to]) {
                        hist = &(*conthist_stack[ply + 1])[board.board[moves[j].from]][moves[j].to];
                        *hist -= bonus + bonus * *hist / MAX_HIST;
                        hist = &(*conthist_stack[ply])[board.board[moves[j].from]][moves[j].to];
                        *hist -= bonus + bonus * *hist / MAX_HIST;
                    }
                }
                int16_t *hist = &history[victim][board.board[moves[i].from]][moves[i].to];
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

        if (!excluded.from && depth && best > LOST + ply) {
            tt.depth = depth;
            tt.score = best;
            tt.bound = best <= orig_alpha ? BOUND_UPPER
                : best >= beta ? BOUND_LOWER
                : BOUND_EXACT;
            tt.mv = tt.bound != BOUND_UPPER ? bestmv
                : tt.key ? Move{}
                : tt.mv;
            tt.key = board.zobrist;
            slot.store(tt, {});
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
        soft_limit = now() + time_alotment * 0.000055;
        Move mv;
        int v = 0;
        try {
            for (int depth = 1; depth <= MAX_DEPTH; depth++) {
                int lower = v;
                int upper = v;
                int delta = 3;
                while (v <= lower || v >= upper) {
                    lower = min(lower - delta, v);
                    upper = max(upper + delta, v);
                    v = negamax(ROOT, mv, lower, upper, depth, 0);
                    delta *= 2.7;
                }
                lock_guard lock(MUTEX);
                if (FINISHED_DEPTH < depth) {
                    FINISHED_DEPTH = depth;
                    BEST_MOVE = mv;
                    cout << "info depth " << depth << " score cp " << v << " pv ";
                    mv.put_with_newline();
                    if (now() > soft_limit) {
                        return;
                    }
                }
            }
        } catch (...) {}
    }
};
