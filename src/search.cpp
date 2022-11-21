double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

struct Searcher {
    uint64_t nodes;
    double abort_time;

    int negamax(Board &board, Move *bestmv, int16_t alpha, int16_t beta, int16_t depth, int ply) {
        if (depth == 0) return board.eval();

        Move moves[256];
        int score[256];
        int mvcount;
        if (!board.movegen(moves, mvcount)) {
            return WON;
        }

        for (int i = 0; i < mvcount; i++) {
            if (board.board[moves[i].to]) {
                score[i] = (board.board[moves[i].to] & 7) * 8 - (board.board[moves[i].from] & 7);
            } else {
                score[i] = -8;
            }
        }

        int16_t best = LOST + ply;

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
            score[i] = tmp2;

            Board mkmove = board;
            mkmove.make_move(moves[i]);
            if (!(++nodes & 0xFFFF) && now() > abort_time) {
                throw 0;
            }
            int16_t v = -negamax(mkmove, 0, -beta, -alpha, depth - 1, ply + 1);
            if (v > best) {
                best = v;
                if (bestmv) *bestmv = moves[i];
            }
            if (v > alpha) alpha = v;
            if (v >= beta) return v;
        }

        return best;
    }

    void iterative_deepening(double time_alotment, int max_depth=250) {
        nodes = 0;
        abort_time = now() + time_alotment * 0.1;
        Move mv;
        try {
            for (int depth = 1; depth <= max_depth; depth++) {
                int16_t v = negamax(ROOT, &mv, LOST, WON, depth, 0);
                printf("info depth %d nodes %ld score cp %d pv ", depth, nodes, v);
                mv.put();
                putchar('\n');
            }
        } catch (...) {}
        printf("bestmove ");
        mv.put();
        putchar('\n');
    }
};
