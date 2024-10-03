
#ifdef OPENBENCH
int THREADS = 1;
#else
// Replaced for TCEC builds by the minifier.
#define THREADS 1
#endif

void uci() {
    setbuf(stdout, 0);
    char buf[4096], *move;
    int wtime, btime;
#ifdef OPENBENCH
    int opt, value;
    TtData empty{};
#endif
    fgets(buf, 4096, stdin); // uci
    printf(
#ifdef OPENBENCH
        "id name ice4 v5\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
#endif
        "uciok\n"
    );
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
                    TT[i] = empty;
                }
                break;
            case 's': // setoption
                strtok(0, " \n"); // name
                opt = *strtok(0, " \n");
                strtok(0, " \n"); // value
                value = atoi(strtok(0, " \n"));
                switch (opt) {
                    case 'H':
                        delete[] TT;
                        TT_SIZE = value * 131072;
                        TT = new atomic<TtData>[TT_SIZE]();
                        break;
                    case 'T':
                        THREADS = value;
                        break;
                }
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
                while (move = strtok(0, " \n")) {
                    PREHISTORY[PREHISTORY_LENGTH++] = ROOT.zobrist;
                    BEST_MOVE = create_move(
                        move[1] * 10 + move[0] - 566,
                        move[3] * 10 + move[2] - 566,
                        !!move[4]
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
                    ROOT.make_move(BEST_MOVE, move[4] % 53 * 5 % 6);
                }
                break;
            case 'g': // go
#ifdef OPENBENCH
                char *w;
                wtime = 1 << 30;
                btime = 1 << 30;
                while (w = strtok(0, " \n")) {
                    if (!strcmp(w, "wtime")) {
                        wtime = atoi(strtok(0, " \n"));
                    } else if (!strcmp(w, "btime")) {
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
