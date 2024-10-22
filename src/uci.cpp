
#ifdef OPENBENCH
int THREADS = 1;
#else
// Replaced for TCEC builds by the minifier.
#define THREADS 1
#endif

void uci() {
    setbuf(stdout, 0);
    char buf[4096], *token;
    int wtime, btime;
    fgets(buf, 4096, stdin); // uci
#ifdef OPENBENCH
    printf(
        "id name ice4 v5\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
    );

#ifdef TUNABLE
    for (auto& [name, variant] : tunableParams) {
        if (auto value = get_if<int*>(&variant)) {
            printf("option name %s type string default %d\n", name.c_str(), **value);
        }
        if (auto value = get_if<float*>(&variant)) {
            printf("option name %s type string default %.17g\n", name.c_str(), **value);
        }
    }
#endif

#endif
    printf("uciok\n");
    for (;;) {
        fgets(buf, 4096, stdin);
        switch (*strtok(buf, " \n")) {
            case 'i': // isready
                printf("readyok\n");
                break;
            case 'q': // quit
                exit(0);
#ifdef OPENBENCH
            case 'u': // ucinewgame
                for (int i = 0; i < TT_SIZE; i++) {
                    TT[i] = TtData{};
                }
                break;
            case 's': // setoption
                strtok(0, " \n"); // name
                token = strtok(0, " \n");
                strtok(0, " \n"); // value
                if (!strcmp(token, "Hash")) {
                    delete[] TT;
                    TT_SIZE = atoi(strtok(0, " \n")) * 131072;
                    TT = new atomic<TtData>[TT_SIZE]();
                }
                if (!strcmp(token, "Threads")) {
                    THREADS = atoi(strtok(0, " \n"));
                } 
#ifdef TUNABLE
                if (tunableParams.count(token)) {
                    auto& variant = tunableParams[token];
                    if (auto value = get_if<int*>(&variant)) {
                        **value = atoi(strtok(0, " \n"));
                    }
                    if (auto value = get_if<float*>(&variant)) {
                        **value = atof(strtok(0, " \n"));
                    }
                }
#endif
                break;
#endif
            case 'p': // position
                ROOT = Board();
#ifdef OPENBENCH
                if (!strcmp(strtok(0, " \n"), "fen")) {
                    parse_fen();
                }
#else
                strtok(0, " \n"); // startpos
#endif
                strtok(0, " \n"); // moves
                PREHISTORY_LENGTH = 0;
                while ((token = strtok(0, " \n"))) {
                    PREHISTORY[PREHISTORY_LENGTH++] = ROOT.zobrist;
                    BEST_MOVE = create_move(
                        token[1] * 10 + token[0] - 566,
                        token[3] * 10 + token[2] - 566,
                        !!token[4]
                    );
                    if ((ROOT.board[BEST_MOVE.from] & 7) == PAWN || ROOT.board[BEST_MOVE.to]) {
                        PREHISTORY_LENGTH = 0;
                    }
                    // maps promotion chars to piece enums
                    //       'q'    'r'    'b'    'n'    '\0' 
                    // cast  113    114    98     110    0
                    // % 53  7      8      45     4      0
                    // * 5   35     40     225    20     0
                    // % 6   5      4      3      2      0
                    // enum  QUEEN  ROOK   BISHOP KNIGHT EMPTY
                    ROOT.make_move(BEST_MOVE, token[4] % 53 * 5 % 6);
                }
                break;
            case 'g': // go
#ifdef OPENBENCH
                wtime = 1 << 30;
                btime = 1 << 30;
                while ((token = strtok(0, " \n"))) {
                    if (!strcmp(token, "wtime")) {
                        wtime = atoi(strtok(0, " \n"));
                    } else if (!strcmp(token, "btime")) {
                        btime = atoi(strtok(0, " \n"));
                    }
                }
#else
                strtok(0, " \n"); // wtime
                wtime = atoi(strtok(0, " \n"));
                strtok(0, " \n"); // btime
                btime = atoi(strtok(0, " \n"));
#endif
                int time_alotment = ROOT.stm == WHITE ? wtime : btime;
                ABORT = 0;
                FINISHED_DEPTH_AND_SCORE = 0;
#ifdef OPENBENCH
                vector<thread> threads(THREADS);
#else
                thread threads[THREADS];
#endif
                for (int i = 0; i < THREADS; i++) {
                    threads[i] = thread([time_alotment]() {
                        Searcher().iterative_deepening(time_alotment);
                        ABORT = 1;
                    });
                }
#ifdef OPENBENCH
                if (wtime == 1 << 30) {
                    while (1) {
                        fgets(buf, 4096, stdin);
                        if (buf[0] == 's') {
                            // stop
                            break;
                        } else if (buf[0] == 'i') {
                            // isready
                            printf("readyok\n");
                        } else if (buf[0] == 'q') {
                            // quit
                            ABORT = 1;
                            for (auto& t : threads) {
                                t.join();
                            }
                            return;
                        }
                    }
                    ABORT = 1;
                }
#endif
                for (int i = 0; i < THREADS; i++) {
                    threads[i].join();
                }
                printf("bestmove ");
                BEST_MOVE.put_with_newline();
        }
    }
}
