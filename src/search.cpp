#define MAX_HIST 4096

double now() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1000000000.0;
}

#define HH_SCALE 1000000
struct HhEntry {
    int32_t value;
    int32_t count;
    HhEntry() : value(HH_SCALE), count(1) {}

    void adjust(int depth) {
        int32_t diff = depth * HH_SCALE - value;
        if (depth && diff < 0) diff = 0;
        value += diff / ++count;
    }
};

struct Searcher {
    uint64_t nodes;
    double abort_time;
    HhEntry piece_to[2][7][SQUARE_SPAN];
    HhEntry from_to[2][SQUARE_SPAN][SQUARE_SPAN];
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
                score[i] = HH_SCALE*100;
            } else if (board.board[moves[i].to]) {
                score[i] = (board.board[moves[i].to] & 7) * 8 - piece + HH_SCALE*99;
            } else if (moves[i] == killers[ply][0] || moves[i] == killers[ply][1]) {
                score[i] = HH_SCALE*98;
            } else {
                score[i] = piece_to[board.stm == BLACK][piece][moves[i].to-A1].value + from_to[board.stm == BLACK][moves[i].from-A1][moves[i].to-A1].value;
            }
        }

        int raised_alpha = 0;

        int16_t best = depth > 0 ? LOST + ply : static_eval;
        if (best >= beta) return best;

        int quiets_to_check_table[] = { 0, 7, 8, 17 };
        int quiets_to_check = depth > 0 && depth < 4 && !pv ? quiets_to_check_table[depth] : 99;

        int legals = 0;
        for (int i = 0; i < mvcount; i++) {
            int best_so_far = i;
            for (int j = i+1; j < mvcount; j++) {
                if (score[j] > score[best_so_far]) {
                    best_so_far = j;
                }
            }
            std::swap(moves[i], moves[best_so_far]);
            std::swap(score[i], score[best_so_far]);

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
                        if (!moves[j].from || board.board[moves[j].to]) continue;
                        piece_to[board.stm == BLACK][board.board[moves[j].from] & 7][moves[j].to-A1].adjust(0);
                        from_to[board.stm == BLACK][moves[j].from-A1][moves[j].to-A1].adjust(0);
                    }
                    piece_to[board.stm == BLACK][board.board[bestmv.from] & 7][bestmv.to-A1].adjust(depth);
                    from_to[board.stm == BLACK][bestmv.from-A1][bestmv.to-A1].adjust(depth);
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
