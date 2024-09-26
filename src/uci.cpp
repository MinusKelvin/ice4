
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
    TtData empty{};
#endif
    fgets(buf, 4096, stdin); // uci
    printf(
#ifdef OPENBENCH
        "id name ice4 v5\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
        "option name aIIR_MIN_DEPTH type string default 3\n"
        "option name bIIR_REDUCTION type string default 1\n"
        "option name cCORRHIST_PAWN_DIV type string default 228\n"
        "option name dCORRHIST_MATERIAL_DIV type string default 228\n"
        "option name eCORRHIST_COUNTERMOVE_DIV type string default 228\n"
        "option name fRFP_MARGIN type string default 44\n"
        "option name gRFP_MAX_DEPTH type string default 7\n"
        "option name hNMP_MIN_DEPTH type string default 1\n"
        "option name iNMP_FACTOR type string default 29\n"
        "option name jNMP_CONSTANT type string default 242\n"
        "option name kNMP_DIVIDOR type string default 88\n"
        "option name lRAZOR_MAX_DEPTH type string default 4\n"
        "option name mRAZOR_MARGIN type string default 50\n"
        "option name nRAZOR_CONSTANT type string default 250\n"
        "option name oIID_MIN_DEPTH type string default 2\n"
        "option name pIID_REDUCTION type string default 6\n"
        "option name qCOUNTER_HIST_FACTOR type string default 2\n"
        "option name rFOLLOWUP_HIST_FACTOR type string default 2.2\n"
        "option name sLMP_CONSTANT type string default 12\n"
        "option name tLMR_MOVES type string default 0.155\n"
        "option name uLMR_DEPTH type string default 0.165\n"
        "option name vLMR_HISTORY type string default 2764\n"
        "option name wHIST_QUAD_UPDATE type string default 4\n"
        "option name xHIST_LINEAR_UPDATE type string default 0\n"
        "option name yHIST_EVAL_MARGIN type string default 30\n"
        "option name zCORRHIST_UPDATE_CONSTANT type string default 3\n"
        "option name 0CORRHIST_UPDATE_MAX type string default 95\n"
        "option name 1CORRHIST_UPDATE_DIVISOR type string default 689\n"
        "option name 2TM_HARD type string default 0.0004\n"
        "option name 3TM_SOFT type string default 0.000043\n"
        "option name 4ASP_INITIAL type string default 7\n"
        "option name 5ASP_WIDEN type string default 1.8\n"
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
                value = atof(strtok(0, " \n"));
                switch (opt) {
                    case 'H':
                        delete[] TT;
                        TT_SIZE = value * 131072;
                        TT = new atomic<TtData>[TT_SIZE]();
                        break;
                    case 'T':
                        THREADS = value;
                        break;
                    case 'a': IIR_MIN_DEPTH = value; break;
                    case 'b': IIR_REDUCTION = value; break;
                    case 'c': CORRHIST_PAWN_DIV = value; break;
                    case 'd': CORRHIST_MATERIAL_DIV = value; break;
                    case 'e': CORRHIST_COUNTERMOVE_DIV = value; break;
                    case 'f': RFP_MARGIN = value; break;
                    case 'g': RFP_MAX_DEPTH = value; break;
                    case 'h': NMP_MIN_DEPTH = value; break;
                    case 'i': NMP_FACTOR = value; break;
                    case 'j': NMP_CONSTANT = value; break;
                    case 'k': NMP_DIVIDOR = value; break;
                    case 'l': RAZOR_MAX_DEPTH = value; break;
                    case 'm': RAZOR_MARGIN = value; break;
                    case 'n': RAZOR_CONSTANT = value; break;
                    case 'o': IID_MIN_DEPTH = value; break;
                    case 'p': IID_REDUCTION = value; break;
                    case 'q': COUNTER_HIST_FACTOR = value; break;
                    case 'r': FOLLOWUP_HIST_FACTOR = value; break;
                    case 's': LMP_CONSTANT = value; break;
                    case 't': LMR_MOVES = value; break;
                    case 'u': LMR_DEPTH = value; break;
                    case 'v': LMR_HISTORY = value; break;
                    case 'w': HIST_QUAD_UPDATE = value; break;
                    case 'x': HIST_LINEAR_UPDATE = value; break;
                    case 'y': HIST_EVAL_MARGIN = value; break;
                    case 'z': CORRHIST_UPDATE_CONSTANT = value; break;
                    case '0': CORRHIST_UPDATE_MAX = value; break;
                    case '1': CORRHIST_UPDATE_DIVISOR = value; break;
                    case '2': TM_HARD = value; break;
                    case '3': TM_SOFT = value; break;
                    case '4': ASP_INITIAL = value; break;
                    case '5': ASP_WIDEN = value; break;
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
                FINISHED_DEPTH = 0;
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
