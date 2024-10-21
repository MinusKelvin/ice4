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
int FINISHED_DEPTH;
Move BEST_MOVE;

typedef int16_t HTable[23][SQUARE_SPAN];

struct Searcher {
    double hard_limit;
    double soft_limit;
    uint64_t nodes;
    uint64_t rep_list[256];
    int mobilities[256];
    HTable history;

    int negamax(Board &board, Move &bestmv, int alpha, int beta, int depth, int ply) {
        Move scratch;
        Move moves[256];
        int score[256];
        int mvcount;
        int pv = beta != alpha + 1;

        depth = depth < 0 ? 0 : depth;
        bestmv = Move{};

        TtData tt = TT[board.zobrist % TT_SIZE].load({});
        int tt_good = tt.key == (uint16_t) (board.zobrist / TT_SIZE);
        if (tt_good && !pv && depth <= tt.depth) {
            if (
                tt.score >= beta && (tt.bound & BOUND_LOWER) ||
                tt.score <= alpha && (tt.bound & BOUND_UPPER)
            ) {
                return tt.score;
            }
        }

        board.movegen(moves, mvcount, depth, mobilities[ply+1]);

        int eval = board.eval(mobilities[ply+1] - mobilities[ply] + TEMPO);

        if (!pv && !board.check && depth < 4 && eval - 50 * depth >= beta) {
            return eval;
        }

        if (!pv && !board.check && depth > 2) {
            Board mkmove = board;
            mkmove.zobrist ^= ZOBRIST[EMPTY][0];
            mkmove.stm ^= INVALID;
            mkmove.ep_square = 0;

            int v = -negamax(mkmove, scratch, -beta, -alpha, depth - 4, ply + 1);
            if (v >= beta) {
                return v;
            }
        }

        for (int j = 0; j < mvcount; j++) {
            if (tt_good && tt.mv.from == moves[j].from && tt.mv.to == moves[j].to) {
                score[j] = 1e7;
            } else if (board.board[moves[j].to]) {
                score[j] = 1e5 * board.board[moves[j].to];
            } else {
                score[j] = history[board.board[moves[j].from]][moves[j].to];
            }
        }

        rep_list[ply] = board.zobrist;

        int best = depth ? LOST + ply : eval;
        int raised_alpha = 0;
        int legals = 0;

        // Quiescence search stand-pat
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

            Board mkmove = board;
            if (mkmove.make_move(moves[i])) {
                continue;
            }

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

            if (is_rep) {
                v = 0;
            } else if (legals) {
                v = -negamax(mkmove, scratch, -alpha-1, -alpha, depth - 1, ply + 1);

                if (pv && v > alpha) {
                    v = -negamax(mkmove, scratch, -beta, -alpha, depth - 1, ply + 1);
                }
            } else {
                v = -negamax(mkmove, scratch, -beta, -alpha, depth - 1, ply + 1);
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
                if (!board.board[moves[i].to]) {
                    int bonus = 64 * depth;
                    for (int j = 0; j < i; j++) {
                        if (!board.board[moves[j].to]) {
                            history[board.board[moves[j].from]][moves[j].to] -=
                                bonus + bonus * history[board.board[moves[j].from]][moves[j].to] / MAX_HIST;
                        }
                    }
                    history[board.board[moves[i].from]][moves[i].to] +=
                        bonus - bonus * history[board.board[moves[i].from]][moves[i].to] / MAX_HIST;
                }
                break;
            }
        }

        if (depth && legals == 0 && !board.check) {
            return 0;
        }

        if (best > LOST + ply) {
            tt.key = board.zobrist / TT_SIZE;
            tt.score = best;
            tt.depth = depth;
            tt.bound = best >= beta ? BOUND_LOWER : raised_alpha ? BOUND_EXACT : BOUND_UPPER;
            tt.mv = bestmv;
            TT[board.zobrist % TT_SIZE].store(tt, {});
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
        hard_limit = now() + time_alotment * 0.0004;
        soft_limit = now() + time_alotment * 0.00005;
        Move mv;
        try {
            for (int depth = 1; depth <= MAX_DEPTH; depth++) {
                int v = negamax(ROOT, mv, LOST, WON, depth, 0);
                lock_guard lock(MUTEX);
                if (FINISHED_DEPTH < depth) {
                    FINISHED_DEPTH = depth;
                    BEST_MOVE = mv;
                    printf("info depth %d score cp %d pv ", depth, v);
                    mv.put_with_newline();
                    if (now() > soft_limit) {
                        return;
                    }
                }
            }
        } catch (...) {}
    }
};
