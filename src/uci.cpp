
#ifdef OPENBENCH
int THREADS = 1;
#else
// Replaced for TCEC builds by the minifier.
#define THREADS 1
#endif

void uci() {
    int wtime, btime;

    string token;
    getline(cin, token);
    cout << unitbuf <<
#ifdef OPENBENCH
        "id name ice4 v5\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
#endif
        "uciok\n";
    while (getline(cin, token)) {
        stringstream tokens(token);
        tokens >> token;
        switch (token[0]) {
            case 'i': // isready
                cout << "readyok\n";
                break;
            case 'q': // quit
                return;
#ifdef OPENBENCH
            case 'u': // ucinewgame
                for (int i = 0; i < TT_SIZE; i++) {
                    TT[i] = TtData{};
                }
                break;
            case 's': // setoption
                tokens >> token >> token; // name <name>
                if (token == "Hash") {
                    delete[] TT;
                    tokens >> token >> TT_SIZE; // value <value>
                    TT_SIZE *= 131072;
                    TT = new atomic<TtData>[TT_SIZE]();
                }
                if (token == "Threads") {
                    tokens >> token >> THREADS; // value <value>
                }
                break;
#endif
            case 'p': // position
                ROOT = Board();
#ifdef OPENBENCH
                tokens >> token; // startpos | fen
                if (token == "fen") {
                    ROOT = parse_fen(tokens);
                }
                tokens >> token; // moves
#else
                tokens >> token >> token; // startpos moves
#endif
                PREHISTORY_LENGTH = 0;
                while (tokens >> token) {
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
                while (tokens >> token) {
                    if (token == "wtime") {
                        tokens >> wtime;
                    }
                    if (token == "btime") {
                        tokens >> btime;
                    }
                }
#else
                tokens >> token >> wtime >> token >> btime; // wtime <wtime> btime <btime>
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
                    while (getline(cin, token)) {
                        if (token == "stop") {
                            break;
                        }
                        if (token == "isready") {
                            cout << "readyok\n";
                        }
                        if (token == "quit") {
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
                cout << "bestmove ";
                BEST_MOVE.put_with_newline();
        }
    }
}
