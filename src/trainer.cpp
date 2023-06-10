struct Datapoint {
    int features[32];
    float target;
};

void train() {
#ifdef OPENBENCH
    auto RNG_FILE = fopen("/dev/urandom", "r");
#endif
    vector<thread> threads;
    vector<Datapoint> data;
    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back([&]() {
            vector<TtEntry> tt(524288);
            vector<Datapoint> game_data;
            Searcher s;
            Move moves[256];
            uint64_t history[64];
            uint64_t movenums[8];
            int mvcount;
            int v;
            int not_done = 1;
            s.tt_ptr = &tt;
            s.prehistory = history;
            s.abort_time = 1.0 / 0.0;
            while (not_done) {
                retry:
                Board board;
                fread(movenums, sizeof(movenums), 1, RNG_FILE);
                for (int j = 0; j < 8; j++) {
                    if (!board.movegen(moves, mvcount)) {
                        goto retry;
                    }
                    board.make_move(moves[movenums[j] % mvcount]);
                }
                s.prehistory_len = 0;
                game_data.clear();
                float outcome;
                for (;;) {
                    Move mv(0);
                    history[s.prehistory_len++] = board.zobrist;
                    for (int depth = 1; depth <= DATAGEN_DEPTH; depth++) {
                        v = s.negamax(board, mv, LOST, WON, depth, 0);
                    }
                    if (v > 20000) {
                        outcome = board.stm == WHITE;
                        break;
                    }
                    if (v < -20000) {
                        outcome = board.stm != WHITE;
                        break;
                    }
                    if (board.board[mv.to] || (board.board[mv.from] & 7) == PAWN) {
                        s.prehistory_len = 0;
                    }
                    if (s.prehistory_len >= 50) {
                        outcome = 0.5;
                        game_data.resize(game_data.size() - 45);
                        break;
                    }
                    Datapoint &elem = game_data.emplace_back();
                    elem.target = v;
                    // TODO: fill feature vector
                    board.make_move(mv);
                }
                int flip = 0;
                for (Datapoint &elem : game_data) {
                    elem.target = (flip - outcome) * OUTCOME_PART +
                        EVAL_PART / (1 + exp(-elem.target / EVAL_SCALE));
                    flip ^= 1;
                }
                MUTEX.lock();
                data.insert(data.end(), game_data.begin(), game_data.end());
                not_done = data.size() < DATAGEN_SIZE;
                // printf("datagen: %ld\n", data.size());
                MUTEX.unlock();
            }
        });
    }
    for (auto& t : threads) {
        t.join();
    }
#ifdef OPENBENCH
    fclose(RNG_FILE);
#endif
}
