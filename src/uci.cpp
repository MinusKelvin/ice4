
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

#ifdef OPENBENCH
    cout <<
        "id name ice4 v6.1\r\n"
        "id author MinusKelvin and analog-hors\n"
        "option name Hash type spin default 8 min 1 max 67108864\n"
        "option name Threads type spin default 1 min 1 max 2048\n"
        "option name CORR_PAWN_DIV type string default 256\n"
        "option name CORR_MAT_DIV type string default 256\n"
        "option name CORR_NONPAWN_DIV type string default 256\n"
        "option name CORR_CONT1_DIV type string default 256\n"
        "option name RFP_MARGIN type string default 38\n"
        "option name LMP_BASE type string default 3\n"
        "option name LMR_BASE type string default -0.11\n"
        "option name LMR_FACTOR type string default 0.69\n"
        "option name LMR_HISTORY type string default 2048\n"
        "option name SINGULAR_MARGIN type string default 2.0\n"
        "option name HISTORY_UPDATE type string default 32\n"
        "option name CORRECTION_MAX type string default 256\n"
        "option name AW_INITIAL type string default 3\n"
        ;
#endif

    cout << "uciok\n";
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
                memset(TT, 0, sizeof(TtData) * (1ull << TT_BITS));
                break;
            case 's': // setoption
                tokens >> token >> token; // name <name>
                if (token == "Hash") {
                    uint64_t tt_size;
                    tokens >> token >> tt_size; // value <value>
                    tt_size *= 1024 * 1024 / sizeof(TtData);

                    free(TT);
                    TT_BITS = 63 - __builtin_clzll(tt_size);
                    TT = (atomic<TtData>* ) calloc(1ull << TT_BITS, sizeof(TtData));
                }
                if (token == "Threads") {
                    tokens >> token >> THREADS; // value <value>
                }
                if (token == "CORR_PAWN_DIV") { tokens >> token >> CORR_PAWN_DIV; }
                if (token == "CORR_MAT_DIV") { tokens >> token >> CORR_MAT_DIV; }
                if (token == "CORR_NONPAWN_DIV") { tokens >> token >> CORR_NONPAWN_DIV; }
                if (token == "CORR_CONT1_DIV") { tokens >> token >> CORR_CONT1_DIV; }
                if (token == "RFP_MARGIN") { tokens >> token >> RFP_MARGIN; }
                if (token == "LMP_BASE") { tokens >> token >> LMP_BASE; }
                if (token == "LMR_BASE") { tokens >> token >> LMR_BASE; }
                if (token == "LMR_FACTOR") { tokens >> token >> LMR_FACTOR; }
                if (token == "LMR_HISTORY") { tokens >> token >> LMR_HISTORY; }
                if (token == "SINGULAR_MARGIN") { tokens >> token >> SINGULAR_MARGIN; }
                if (token == "HISTORY_UPDATE") { tokens >> token >> HISTORY_UPDATE; }
                if (token == "CORRECTION_MAX") { tokens >> token >> CORRECTION_MAX; }
                if (token == "AW_INITIAL") { tokens >> token >> AW_INITIAL; }
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
                FINISHED_DEPTH = 0;
#ifdef OPENBENCH
                vector<thread> threads(THREADS);
#else
                thread threads[THREADS];
#endif
                for (thread& t : threads) {
                    t = thread([time_alotment]() {
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
                            cout << "readyok" << endl;
                        }
                        if (token == "quit") {
                            ABORT = 1;
                            for (thread& t : threads) {
                                t.join();
                            }
                            return;
                        }
                    }
                    ABORT = 1;
                }
#endif
                for (thread& t : threads) {
                    t.join();
                }
                cout << "bestmove ";
                BEST_MOVE.put_with_newline();
        }
    }
}
