struct Datapoint {
    int features[32];
    float target;
};

void datagen(vector<Datapoint> &data) {
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
            if (s.prehistory_len >= 50 || !mv.from) {
                outcome = 0.5;
                break;
            }
            Datapoint &elem = game_data.emplace_back();
            elem.target = v;
            int flip = board.stm == WHITE ? 0 : FEATURE_FLIP;
            int i = 0;
            for (int sq = A1; sq <= H8; sq++) {
                if (board.board[sq] & 7) {
                    elem.features[i++] = FEATURE[board.board[sq]][sq-A1] ^ flip;
                }
            }
            while (i < 32) {
                elem.features[i++] = -1;
            }
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
}

void train() {
    int32_t random[772][NEURONS];
    fread(random, sizeof(random), 1, RNG_FILE);
    for (int i = 0; i < 768; i++) {
        if (!i || i == FEATURE_FLIP) {
            continue;
        }
        for (int j = 0; j < NEURONS; j++) {
            FT[i][j] = FT_INIT_SCALE * random[i][j] / (float)(1 << 31);
        }
    }
    for (int i = 0; i < NEURONS; i++) {
        FT_BIAS[i] = FT_INIT_SCALE * random[768][i] / (float)(1 << 31);
        OUT[i] = OUT_INIT_SCALE * random[769][i] / (float)(1 << 31);
        OUT[i+NEURONS] = OUT_INIT_SCALE * random[770][i] / (float)(1 << 31);
    }
    OUT_BIAS = OUT_INIT_SCALE * random[771][0] / (float)(1 << 31);

    vector<thread> threads;
    vector<Datapoint> data;
    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back([&]() {
            datagen(data);
        });
    }
    for (auto& t : threads) {
        t.join();
    }
}
