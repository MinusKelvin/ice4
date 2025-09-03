
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
        "id name ice4 v6\r\n"
        "id author MinusKelvin and analog-hors\n"
        "option name Hash type spin default 8 min 1 max 67108864\n"
        "option name Threads type spin default 1 min 1 max 2048\n"
"option name CORR_HIST_UNIT type string default 228\n"
"option name CORR_HIST_DIV type string default 456\n"
"option name CORR_HIST_MAX type string default 72\n"
"option name HARD_TM type string default 0.4\n"
"option name SOFT_TM type string default 0.054\n"
"option name CORR_PAWN_DIV type string default 178\n"
"option name CORR_MAT_DIV type string default 198\n"
"option name CORR_NONPAWN_DIV type string default 256\n"
"option name CORR_1PLY_DIV type string default 102\n"
"option name CORR_2PLY_DIV type string default 200\n"
"option name RFP_MARGIN type string default 43\n"
"option name NMP_MUL type string default 27\n"
"option name NMP_DIV type string default 107\n"
"option name NMP_CONST type string default 438\n"
"option name RAZOR_MUL type string default 65\n"
"option name RAZOR_CONST type string default 195\n"
"option name CMHIST_FACTOR type string default 2\n"
"option name FMHIST_FACTOR type string default 2.4\n"
"option name LMR_MUL type string default 0.65\n"
"option name LMR_CONST type string default 0.33\n"
"option name HRED_DIV type string default 3842\n"
"option name CAP_HRED_DIV type string default 3842\n"
"option name HIST_BONUS_MUL type string default 56\n"
"option name HIST_EXTRA_MARGIN type string default 42\n"
"option name CORR_WEIGHT_MAX type string default 93\n"
"option name CORR_WEIGHT_DIV type string default 591.0\n"
"option name OPT_NUMER_CONST type string default 50\n"
"option name OPT_DENOM_CONST type string default 100\n"
"option name DELTA_PROMO type string default 717\n"
"option name DELTA_PAWN type string default 100\n"
"option name DELTA_KNIGHT type string default 423\n"
"option name DELTA_BISHOP type string default 569\n"
"option name DELTA_ROOK type string default 755\n"
"option name DELTA_QUEEN type string default 1280\n";
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
else if (token == "CORR_HIST_UNIT") { tokens >> token >> CORR_HIST_UNIT; }
else if (token == "CORR_HIST_DIV") { tokens >> token >> CORR_HIST_DIV; }
else if (token == "CORR_HIST_MAX") { tokens >> token >> CORR_HIST_MAX; }
else if (token == "HARD_TM") { tokens >> token >> HARD_TM; }
else if (token == "SOFT_TM") { tokens >> token >> SOFT_TM; }
else if (token == "CORR_PAWN_DIV") { tokens >> token >> CORR_PAWN_DIV; }
else if (token == "CORR_MAT_DIV") { tokens >> token >> CORR_MAT_DIV; }
else if (token == "CORR_NONPAWN_DIV") { tokens >> token >> CORR_NONPAWN_DIV; }
else if (token == "CORR_1PLY_DIV") { tokens >> token >> CORR_1PLY_DIV; }
else if (token == "CORR_2PLY_DIV") { tokens >> token >> CORR_2PLY_DIV; }
else if (token == "RFP_MARGIN") { tokens >> token >> RFP_MARGIN; }
else if (token == "NMP_MUL") { tokens >> token >> NMP_MUL; }
else if (token == "NMP_DIV") { tokens >> token >> NMP_DIV; }
else if (token == "NMP_CONST") { tokens >> token >> NMP_CONST; }
else if (token == "RAZOR_MUL") { tokens >> token >> RAZOR_MUL; }
else if (token == "RAZOR_CONST") { tokens >> token >> RAZOR_CONST; }
else if (token == "CMHIST_FACTOR") { tokens >> token >> CMHIST_FACTOR; }
else if (token == "FMHIST_FACTOR") { tokens >> token >> FMHIST_FACTOR; }
else if (token == "LMR_MUL") { tokens >> token >> LMR_MUL; }
else if (token == "LMR_CONST") { tokens >> token >> LMR_CONST; }
else if (token == "HRED_DIV") { tokens >> token >> HRED_DIV; }
else if (token == "CAP_HRED_DIV") { tokens >> token >> CAP_HRED_DIV; }
else if (token == "HIST_BONUS_MUL") { tokens >> token >> HIST_BONUS_MUL; }
else if (token == "HIST_EXTRA_MARGIN") { tokens >> token >> HIST_EXTRA_MARGIN; }
else if (token == "CORR_WEIGHT_MAX") { tokens >> token >> CORR_WEIGHT_MAX; }
else if (token == "CORR_WEIGHT_DIV") { tokens >> token >> CORR_WEIGHT_DIV; }
else if (token == "OPT_NUMER_CONST") { tokens >> token >> OPT_NUMER_CONST; }
else if (token == "OPT_DENOM_CONST") { tokens >> token >> OPT_DENOM_CONST; }
else if (token == "DELTA_PROMO") { tokens >> token >> DELTA_PROMO; }
else if (token == "DELTA_PAWN") { tokens >> token >> DELTA_PAWN; }
else if (token == "DELTA_KNIGHT") { tokens >> token >> DELTA_KNIGHT; }
else if (token == "DELTA_BISHOP") { tokens >> token >> DELTA_BISHOP; }
else if (token == "DELTA_ROOK") { tokens >> token >> DELTA_ROOK; }
else if (token == "DELTA_QUEEN") { tokens >> token >> DELTA_QUEEN; }
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
