#define MAX_HIST 4096
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
    uint64_t nodes;
    double hard_limit;
    double soft_limit;
    uint64_t rep_list[256];
    int mobilities[256];

    int negamax(Board &board, Move &bestmv, int alpha, int beta, int depth, int ply) {
        Move scratch, hashmv{};
        Move moves[256];
        int score[256];
        int mvcount;

        depth = depth < 0 ? 0 : depth;

        board.movegen(moves, mvcount, depth, mobilities[ply+1]);

        int eval = board.eval(mobilities[ply+1] - mobilities[ply] + TEMPO);

        for (int j = 0; j < mvcount; j++) {
            score[j] = board.board[moves[j].to];
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
                break;
            }
        }

        if (depth && legals == 0 && !board.check) {
            return 0;
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
