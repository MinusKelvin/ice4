
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
    int opt;
    double value;
#endif
    fgets(buf, 4096, stdin); // uci
    printf(
#ifdef OPENBENCH
        "id name ice4 v4\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
        "option name aIIR_MIN_DEPTH type string default 3\n"
        "option name bRFP_MAX_DEPTH type string default 7\n"
        "option name cRFP_MARGIN type string default 77\n"
        "option name dNMP_MIN_DEPTH type string default 1\n"
        "option name eNMP_DIVISOR type string default 76\n"
        "option name fNMP_DEPTH_FACTOR type string default 29\n"
        "option name gNMP_CONSTANT type string default 228\n"
        "option name hIID_MIN_DEPTH type string default 1\n"
        "option name iIID_REDUCTION type string default 5\n"
        "option name jCMH_CONTRIB type string default 2\n"
        "option name kFMH_CONTRIB type string default 3\n"
        "option name lLMP_FACTOR type string default 1\n"
        "option name nLMP_C type string default 4\n"
        "option name pDELTA_NONE type string default 814\n"
        "option name qDELTA_PAWN type string default 139\n"
        "option name rDELTA_KNIGHT type string default 344\n"
        "option name sDELTA_BISHOP type string default 403\n"
        "option name tDELTA_ROOK type string default 649\n"
        "option name uDELTA_QUEEN type string default 867\n"
        "option name vPPP_DEPTH_FACTOR type string default 0.5\n"
        "option name wLMR_MOVES type string default 0.114\n"
        "option name xLMR_DEPTH type string default 0.152\n"
        "option name 0LMR_BASE type string default 0.0\n"
        "option name yLMR_HISTORY type string default 580\n"
        "option name zCORR_DIV type string default 1\n"
        "option name 1CORR_C type string default 4\n"
        "option name 2CORR_MAX type string default 128\n"
        "option name 3CORR_HIST_UNIT type string default 256\n"
        "option name 4TM_HARD type string default 0.5\n"
        "option name 5TM_SOFT type string default 0.04\n"
        "option name 6ASP_INITIAL type string default 7\n"
        "option name 7ASP_EXPAND type string default 2\n"
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
            case 's': // setoption
                strtok(0, " \n"); // name
                opt = *strtok(0, " \n");
                strtok(0, " \n"); // value
                value = atof(strtok(0, " \n"));
                switch (opt) {
                    case 'H':
                        TT = vector< atomic<TtData> >(value * 131072);
                        break;
                    case 'T':
                        THREADS = value;
                        break;
                    case 'a': IIR_MIN_DEPTH = value; break;
                    case 'b': RFP_MAX_DEPTH = value; break;
                    case 'c': RFP_MARGIN = value; break;
                    case 'd': NMP_MIN_DEPTH = value; break;
                    case 'e': NMP_DIVISOR = value; break;
                    case 'f': NMP_DEPTH_FACTOR = value; break;
                    case 'g': NMP_CONSTANT = value; break;
                    case 'h': IID_MIN_DEPTH = value; break;
                    case 'i': IID_REDUCTION = value; break;
                    case 'j': CMH_CONTRIB = value; break;
                    case 'k': FMH_CONTRIB = value; break;
                    case 'l': LMP_FACTOR = value; break;
                    case 'n': LMP_C = value; break;
                    case 'p': DELTA_NONE = value; break;
                    case 'q': DELTA_PAWN = value; break;
                    case 'r': DELTA_KNIGHT = value; break;
                    case 's': DELTA_BISHOP = value; break;
                    case 't': DELTA_ROOK = value; break;
                    case 'u': DELTA_QUEEN = value; break;
                    case 'v': PPP_DEPTH_FACTOR = value; break;
                    case 'w': LMR_MOVES = value; break;
                    case 'x': LMR_DEPTH = value; break;
                    case '0': LMR_BASE = value; break;
                    case 'y': LMR_HISTORY = value; break;
                    case 'z': CORR_DIV = value; break;
                    case '1': CORR_C = value; break;
                    case '2': CORR_MAX = value; break;
                    case '3': CORR_HIST_UNIT = value; break;
                    case '4': TM_HARD = value; break;
                    case '5': TM_SOFT = value; break;
                    case '6': ASP_INITIAL = value; break;
                    case '7': ASP_EXPAND = value; break;
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
                    Move mv(
                        move[1] * 10 + move[0] - 566,
                        move[3] * 10 + move[2] - 566,
                        !!move[4]
                    );
                    if ((ROOT.board[mv.from] & 7) == PAWN || ROOT.board[mv.to]) {
                        PREHISTORY_LENGTH = 0;
                    }
                    // maps promotion chars to piece enums
                    //       'q'    'r'    'b'    'n'    '\0' 
                    // cast  113    114    98     110    0
                    // % 53  7      8      45     4      0
                    // * 5   35     40     225    20     0
                    // % 6   5      4      3      2      0
                    // enum  QUEEN  ROOK   BISHOP KNIGHT EMPTY
                    ROOT.make_move(mv, move[4] % 53 * 5 % 6);
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
                double time_alotment = (ROOT.stm == WHITE ? wtime : btime) / 1e3;
                ABORT = 0;
                FINISHED_DEPTH = 0;
                vector<thread> threads;
                for (int i = 0; i < THREADS; i++) {
                    threads.emplace_back([time_alotment]() {
                        Searcher s;
                        s.iterative_deepening(time_alotment);
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
                        } else if (buf[1] == 'q') {
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
