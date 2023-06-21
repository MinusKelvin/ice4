struct Datapoint {
    int features[33];
    float target;
};

float sigmoid(float x) {
    return 1 / (1 + exp(-x));
}

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
            elem.features[i] = -1;
            board.make_move(mv);
        }
        int flip = 0;
        for (Datapoint &elem : game_data) {
            elem.target = (flip - outcome) * OUTCOME_PART +
                EVAL_PART * sigmoid(elem.target / EVAL_SCALE);
            flip ^= 1;
        }
        MUTEX.lock();
        data.insert(data.end(), game_data.begin(), game_data.end());
        not_done = data.size() < DATAGEN_SIZE;
        printf("datagen: %ld\n", data.size());
        MUTEX.unlock();
    }
}

void optimize(vector<Datapoint> &data, int &index) {
    MUTEX.lock();
    while (index < data.size()) {
        int start = index;
        int end = min((int) data.size(), index += 1024);
        Nnue weights = NNUE;
        MUTEX.unlock();

        Nnue grad_acc = {};
        float batch_loss = 0;
        for (int i = start; i < end; i++) {
            Nnue grad = {}; // dv_dparam for output layer, dhidden_dparam for ft
            float dv_dhidden[2][NEURONS];
            float hidden[2][NEURONS];
            memcpy(hidden[0], weights.ft_bias, sizeof(weights.ft_bias));
            memcpy(hidden[1], weights.ft_bias, sizeof(weights.ft_bias));
            for (int k = 0; k < NEURONS; k++) {
                grad.ft_bias[k] = 1; // dhidden_dparam = 1
            }
            for (int j = 0; data[i].features[j] != -1; j++) {
                for (int k = 0; k < NEURONS; k++) {
                    hidden[0][k] += weights.ft[data[i].features[j]][k];
                    grad.ft[data[i].features[j]][k] += 1; // dhidden_dparam

                    hidden[1][k] += weights.ft[data[i].features[j] ^ FEATURE_FLIP][k];
                    grad.ft[data[i].features[j] ^ FEATURE_FLIP][k] += 1; // dhidden_dparam
                }
            }
            float v = weights.out_bias;
            grad.out_bias = 1;
            for (int i = 0; i < NEURONS; i++) {
                float activated = max(hidden[0][i], 0.f);
                float dactivated_dhidden = hidden[0][i] > 0;
                v += weights.out[i] * activated;
                grad.out[i] = activated; // dv_dparam = activated
                float dv_dactivated = weights.out[i];
                dv_dhidden[0][i] = dv_dactivated * dactivated_dhidden;

                activated = max(hidden[1][i], 0.f);
                dactivated_dhidden = hidden[1][i] > 0;
                v += weights.out[i+NEURONS] * activated;
                grad.out[i+NEURONS] = activated; // dv_dparam = activated
                dv_dactivated = weights.out[i+NEURONS];
                dv_dhidden[1][i] = dv_dactivated * dactivated_dhidden;
            }
            float activated = sigmoid(v);
            float dactivated_dv = sigmoid(v) * (1 - sigmoid(v));
            float difference = data[i].target - activated;
            float ddifference_dactivated = -1;
            float loss = difference * difference;
            float dloss_ddifference = 2 * difference;

            float dloss_dv = dloss_ddifference * ddifference_dactivated * dactivated_dv;

            grad_acc.out_bias += dloss_dv * grad.out_bias; // dloss_dparam = dloss_dv * dv_dparam
            for (int i = 0; i < NEURONS_X2; i++) {
                grad_acc.out[i] += dloss_dv * grad.out[i]; // dloss_dparam = dloss_dv * dv_dparam
            }

            float dloss_dhidden[NEURONS];
            for (int i = 0; i < NEURONS; i++) {
                dloss_dhidden[i] = dloss_dv * (dv_dhidden[0][i] + dv_dhidden[1][i]); // dloss_dhidden = dloss_dv * dv_dhidden
                grad_acc.ft_bias[i] += dloss_dhidden[i] * grad.ft_bias[i];
                // dloss_dparam = dloss_dhidden * dhidden_dparam
            }
            for (int j = 0; data[i].features[j] != -1; j++) {
                for (int k = 0; k < NEURONS; k++) {
                    grad_acc.ft[data[i].features[j]][k] += dloss_dhidden[k] * grad.ft[data[i].features[j]][k]; // dloss_dparam = dloss_dhidden * dhidden_dparam
                }
            }

            batch_loss += loss;
        }

        MUTEX.lock();

        printf("batch loss: %g\n", batch_loss / (end - start));

        float factor = LR / (end - start);
        for (int j = 0; j < 768; j++) {
            for (int k = 0; k < NEURONS; k++) {
                NNUE.ft[j][k] -= grad_acc.ft[j][k] * factor;
            }
        }
        for (int k = 0; k < NEURONS; k++) {
            NNUE.ft_bias[k] -= grad_acc.ft_bias[k] * factor;
            NNUE.out[k] -= grad_acc.out[k] * factor;
            NNUE.out[k+NEURONS] -= grad_acc.out[k+NEURONS] * factor;
        }
        NNUE.out_bias -= grad_acc.out_bias * factor;
    }
    MUTEX.unlock();
}

void train() {
    int32_t random[772][NEURONS];
    fread(random, sizeof(random), 1, RNG_FILE);
    for (int i = 0; i < 768; i++) {
        if (!i || i == FEATURE_FLIP) {
            continue;
        }
        for (int j = 0; j < NEURONS; j++) {
            NNUE.ft[i][j] = FT_INIT_SCALE * random[i][j] / (float)(1 << 31);
        }
    }
    for (int i = 0; i < NEURONS; i++) {
        NNUE.ft_bias[i] = FT_INIT_SCALE * random[768][i] / (float)(1 << 31);
        NNUE.out[i] = OUT_INIT_SCALE * random[769][i] / (float)(1 << 31);
        NNUE.out[i+NEURONS] = OUT_INIT_SCALE * random[770][i] / (float)(1 << 31);
    }
    NNUE.out_bias = OUT_INIT_SCALE * random[771][0] / (float)(1 << 31);

    auto cycle = [&]() {
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

        vector<uint64_t> shuffle(data.size());
        fread(shuffle.data(), sizeof(uint64_t) * data.size(), 1, RNG_FILE);
        for (int i = 0; i < data.size(); i++) {
            swap(data[i], data[shuffle[i] % (data.size() - i) + i]);
        }

        for (int j = 0; j < 20; j++) {
            threads.clear();
            int index = 0;
            for (int i = 0; i < THREADS; i++) {
                threads.emplace_back([&]() {
                    optimize(data, index);
                });
            }
            for (auto& t : threads) {
                t.join();
            }
        }
    };
    cycle();
    cycle();

#ifdef OPENBENCH
    FILE *outfile = fopen("network.txt", "wb");
    fprintf(outfile, "{");
    
    // ft
    fprintf(outfile, "{");
    for (int i = 0; i < 768; i++) {
        fprintf(outfile, "{");
        for (int j = 0; j < NEURONS; j++) {
            fprintf(outfile, "%g,", NNUE.ft[i][j]);
        }
        fprintf(outfile, "},\n");
    }
    fprintf(outfile, "},\n");
    
    // ft_bias
    fprintf(outfile, "{");
    for (int j = 0; j < NEURONS; j++) {
        fprintf(outfile, "%g,", NNUE.ft_bias[j]);
    }
    fprintf(outfile, "},\n");
    
    // out
    fprintf(outfile, "{");
    for (int j = 0; j < NEURONS_X2; j++) {
        fprintf(outfile, "%g,", NNUE.out[j]);
    }
    fprintf(outfile, "},\n");

    // out_bias
    fprintf(outfile, "%g", NNUE.out_bias);

    fprintf(outfile, "}");
    fclose(outfile);
#endif
}
