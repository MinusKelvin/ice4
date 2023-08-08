struct Datapoint {
    int features[33];
    float target;
};

struct Trainer {
    vector<Datapoint> data;
    barrier<> bar;
#ifdef OPENBENCH
    float total_loss;
#endif
    float lr;
    float outcome_part;
    size_t index;
    size_t datagen_size;
    int datagen_depth;
    float grad[12337], sum_grad_sq[12337];

    Trainer() : data(), bar(THREADS) {}

    float sigmoid(float x) {
        return 1 / (1 + exp(-x));
    }

    void datagen() {
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
                for (int depth = 1; depth <= datagen_depth; depth++) {
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
                int flip = board.stm != WHITE;
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
                elem.target = (flip ? 1 - outcome : outcome) * outcome_part +
                    (1 - outcome_part) * sigmoid(elem.target / EVAL_SCALE);
                flip ^= 1;
            }
            MUTEX.lock();
            data.insert(data.end(), game_data.begin(), game_data.end());
            not_done = data.size() < datagen_size;
#ifdef OPENBENCH
            if (data.size() / 10000 != (data.size() - game_data.size()) / 10000) {
                printf("datagen: %ld\n", data.size());
            }
#endif
            MUTEX.unlock();
        }
    }

    void optimize() {
        while (index < data.size()) {
            int start_index = index;
            bar.arrive_and_wait();
            MUTEX.lock();
            int start = min(data.size(), index);
            int end = min(data.size(), index += BATCH_SIZE / THREADS);
            MUTEX.unlock();

            Nnue grad_acc = {};
#ifdef OPENBENCH
            float batch_loss = 0;
#endif
            for (int i = start; i < end; i++) {
                Datapoint elem = data[i];
                float dv_dout[NEURONS_X2]; // dv_dparam for output layer
                float dv_dhidden[NEURONS_X2];
                float hidden[NEURONS_X2];
                float dloss_dhidden[2][NEURONS];
                float v = NNUE.out_bias;
                memcpy(hidden, NNUE.ft_bias, sizeof(NNUE.ft_bias));
                memcpy(&hidden[NEURONS], NNUE.ft_bias, sizeof(NNUE.ft_bias));
                for (int j = 0; elem.features[j] != -1; j++) {
                    for (int k = 0; k < NEURONS; k++) {
                        hidden[k] += NNUE.ft[elem.features[j]][k];
                        hidden[k+NEURONS] += NNUE.ft[elem.features[j] ^ FEATURE_FLIP][k];
                    }
                }
                for (int i = 0; i < NEURONS_X2; i++) {
                    dv_dout[i] = max(hidden[i], 0.f); // dv_dparam = activated
                    v += NNUE.out[i] * dv_dout[i];
                    dv_dhidden[i] = NNUE.out[i] * (hidden[i] > 0);
                }

#ifdef OPENBENCH
                float difference = elem.target - sigmoid(v);
                batch_loss += difference * difference;
#endif

                float dloss_dv = (elem.target - sigmoid(v)) * sigmoid(v) * (1 - sigmoid(v));

                grad_acc.out_bias += dloss_dv; // dloss_dparam = dloss_dv * dv_dparam
                for (int i = 0; i < NEURONS_X2; i++) {
                    grad_acc.out[i] += dloss_dv * dv_dout[i]; // dloss_dparam = dloss_dv * dv_dparam
                }

                for (int i = 0; i < NEURONS; i++) {
                    dloss_dhidden[0][i] = dloss_dv * dv_dhidden[i];
                    dloss_dhidden[1][i] = dloss_dv * dv_dhidden[i+NEURONS];
                    grad_acc.ft_bias[i] += dloss_dhidden[0][i] + dloss_dhidden[1][i];
                    // dloss_dparam = dloss_dhidden * dhidden_dparam
                }
                for (int j = 0; elem.features[j] != -1; j++) {
                    for (int k = 0; k < NEURONS; k++) {
                        grad_acc.ft[elem.features[j]][k] += dloss_dhidden[0][k]; // dloss_dparam = dloss_dhidden * dhidden_dparam
                        grad_acc.ft[elem.features[j] ^ FEATURE_FLIP][k] += dloss_dhidden[1][k]; // dloss_dparam = dloss_dhidden * dhidden_dparam
                    }
                }
            }

            MUTEX.lock();

#ifdef OPENBENCH
            total_loss += batch_loss;
#endif

            for (int i = 0; i < 12337; i++) {
                grad[i] += ((float*)&grad_acc)[i];
            }
            MUTEX.unlock();

            bar.arrive_and_wait();

            if (start_index == start) {
                for (int i = 0; i < 12337; i++) {
                    sum_grad_sq[i] += grad[i] * grad[i];
                    ((float*)&NNUE)[i] += lr * grad[i] / (sqrt(sum_grad_sq[i]) + 1e-8);
                }

                memset(grad, 0, sizeof(grad));
            }
        }
    }
};

#ifdef OPENBENCH
void write_network(const char *file) {
    FILE *outfile = fopen(file, "wb");
    fprintf(outfile, "{");
    
    // ft
    fprintf(outfile, "{");
    for (int i = 0; i < 768; i++) {
        fprintf(outfile, "{");
        for (int j = 0; j < NEURONS; j++) {
            fprintf(outfile, "%d,", QNNUE.ft[i][j]);
        }
        fprintf(outfile, "},\n");
    }
    fprintf(outfile, "},\n");
    
    // ft_bias
    fprintf(outfile, "{");
    for (int j = 0; j < NEURONS; j++) {
        fprintf(outfile, "%d,", QNNUE.ft_bias[j]);
    }
    fprintf(outfile, "},\n");
    
    // out
    fprintf(outfile, "{");
    for (int j = 0; j < NEURONS_X2; j++) {
        fprintf(outfile, "%d,", QNNUE.out[j]);
    }
    fprintf(outfile, "},\n");

    // out_bias
    fprintf(outfile, "%d", QNNUE.out_bias);

    fprintf(outfile, "}");
    fclose(outfile);
}
#endif

void install_net() {
    for (int f = 0; f < 768; f++) {
        for (int i = 0; i < NEURONS; i++) {
            QNNUE.ft[f][i] = round(NNUE.ft[f][i] * 127);
        }
    }
    for (int i = 0; i < NEURONS; i++) {
        QNNUE.ft_bias[i] = round(NNUE.ft_bias[i] * 127);
        QNNUE.out[i] = round(NNUE.out[i] * 64);
        QNNUE.out[i+NEURONS] = round(NNUE.out[i+NEURONS] * 64);
    }
    QNNUE.out_bias = round(NNUE.out_bias * 127 * 64);
}

void train() {
    auto parallel = [&](auto f) {
        vector<thread> threads;
        for (int i = 0; i < THREADS; i++) {
            threads.emplace_back(f);
        }
        for (auto &t : threads) {
            t.join();
        }
    };

    int32_t random[768][NEURONS];
    fread(random, sizeof(random), 1, RNG_FILE);
    for (int i = 2; i < 768; i++) {
        if (!(i & 1)) {
            NNUE.ft[i][0] += 0.5;
        }
        for (int j = 0; j < NEURONS; j++) {
            NNUE.ft[i][j] += FT_INIT_SCALE * random[i][j] / 0x1p31;
        }
    }
    NNUE.out[0] = 1;
    NNUE.out[NEURONS] = -1;
    for (int i = 0; i < NEURONS; i++) {
        NNUE.ft_bias[i] = FT_INIT_SCALE * random[0][i] / 0x1p31;
        NNUE.out[i] += OUT_INIT_SCALE * random[1][i] / 0x1p31;
        NNUE.out[i+NEURONS] += OUT_INIT_SCALE * random[2][i] / 0x1p31;
    }
    NNUE.out_bias = OUT_INIT_SCALE * random[3][0] / 0x1p31;
    install_net();

    Trainer trainer;

    auto cycle = [&]() {
        parallel([&]() { trainer.datagen(); });

        vector<uint64_t> shuffle(trainer.data.size());
        fread(shuffle.data(), sizeof(uint64_t) * trainer.data.size(), 1, RNG_FILE);
        for (int i = 0; i < trainer.data.size(); i++) {
            swap(trainer.data[i], trainer.data[shuffle[i] % (trainer.data.size() - i) + i]);
        }

        trainer.index = 0;
#ifdef OPENBENCH
        trainer.total_loss = 0;
#endif
        parallel([&]() { trainer.optimize(); });
#ifdef OPENBENCH
        printf("loss: %g\n", trainer.total_loss / trainer.data.size());
#endif

        install_net();
    };
    trainer.lr = 0.01;
    trainer.datagen_depth = 5;
    trainer.datagen_size = 10000;
    trainer.outcome_part = 1;
    memset(trainer.grad, 0, sizeof(trainer.grad));
    memset(trainer.sum_grad_sq, 0, sizeof(trainer.sum_grad_sq));

#ifdef OPENBENCH
    write_network("networks/0.txt");
    double start = now();
#endif

    for (int i = 0; i < 4000; i++) {
        trainer.data.clear();
        cycle();

#ifdef OPENBENCH
        printf("iter %d done\n", i+1);
        if ((i + 1) % 10 == 0) {
            int ms = (now() - start) * 1000;
            char buf[256];
            sprintf(buf, "networks/%d.txt", ms);
            write_network(buf);
        }
#endif
    }

#ifdef OPENBENCH
    write_network("network.txt");
#endif
}
