
#ifdef OPENBENCH
void parse_fen();
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
#endif
    fgets(buf, 4096, stdin); // uci
    printf(
#ifdef OPENBENCH
        "id name ice4 v3.1\r\n"
        "id author MinusKelvin\n"
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
        "option name aLMR_BASE type spin default 0 min -10000 max 10000\n"
        "option name bLMR_MOVE type spin default 114 min -10000 max 10000\n"
        "option name cLMR_DEPTH type spin default 152 min -10000 max 10000\n"
        "option name dLMR_PV type spin default 0 min -10000 max 10000\n"
        "option name eLMR_IMPROVING type spin default 0 min -10000 max 10000\n"
        "option name fLMR_EXTRA type spin default 7 min -10000 max 10000\n"
        "option name gLMR_HIST type spin default 580 min -10000 max 10000\n"
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
                value = atoi(strtok(0, " \n"));
                switch (opt) {
                    case 'H':
                        TT = vector<atomic<TtData>>(value * 131072);
                        break;
                    case 'T':
                        THREADS = value;
                        break;
                    case 'a':
                        LMR_BASE = value;
                        break;
                    case 'b':
                        LMR_MOVE = value;
                        break;
                    case 'c':
                        LMR_DEPTH = value;
                        break;
                    case 'd':
                        LMR_PV = value;
                        break;
                    case 'e':
                        LMR_IMPROVING = value;
                        break;
                    case 'f':
                        LMR_EXTRA = value;
                        break;
                    case 'g':
                        LMR_HIST = value;
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

#ifdef OPENBENCH
void parse_fen() {
    int rank = 7;
    int file = 0;
    char *word = strtok(0, " \n");
    for (char c = *word++; c; c = *word++) {
        int sq = (rank * 10) + file + A1;
        file++;
        switch (c) {
            case 'P':
                ROOT.edit(sq, WHITE | PAWN);
                break;
            case 'N':
                ROOT.edit(sq, WHITE | KNIGHT);
                break;
            case 'B':
                ROOT.edit(sq, WHITE | BISHOP);
                break;
            case 'R':
                ROOT.edit(sq, WHITE | ROOK);
                break;
            case 'Q':
                ROOT.edit(sq, WHITE | QUEEN);
                break;
            case 'K':
                ROOT.edit(sq, WHITE | KING);
                break;
            case 'p':
                ROOT.edit(sq, BLACK | PAWN);
                break;
            case 'n':
                ROOT.edit(sq, BLACK | KNIGHT);
                break;
            case 'b':
                ROOT.edit(sq, BLACK | BISHOP);
                break;
            case 'r':
                ROOT.edit(sq, BLACK | ROOK);
                break;
            case 'q':
                ROOT.edit(sq, BLACK | QUEEN);
                break;
            case 'k':
                ROOT.edit(sq, BLACK | KING);
                break;
            case '/':
                file = 0;
                rank--;
                break;
            default:
                file += c - '1';
                for (int i = 0; i < c - '0'; i++) {
                    ROOT.edit(sq+i, 0);
                }
                break;
        }
    }

    if (*strtok(0, " \n") == 'b') {
        ROOT.stm = BLACK;
        ROOT.zobrist ^= ZOBRIST.stm;
    }

    word = strtok(0, " \n");
    int remove_white_short = 1;
    int remove_white_long = 1;
    int remove_black_short = 1;
    int remove_black_long = 1;
    for (char c = *word++; c; c = *word++) {
        switch (c) {
            case 'K':
                remove_white_short = 0;
                break;
            case 'Q':
                remove_white_long = 0;
                break;
            case 'k':
                remove_black_short = 0;
                break;
            case 'q':
                remove_black_long = 0;
                break;
        }
    }
    if (remove_white_short) ROOT.remove_castle_rights(0, SHORT_CASTLE);
    if (remove_white_long) ROOT.remove_castle_rights(0, LONG_CASTLE);
    if (remove_black_short) ROOT.remove_castle_rights(1, SHORT_CASTLE);
    if (remove_black_long) ROOT.remove_castle_rights(1, LONG_CASTLE);

    word = strtok(0, " \n");
    if (*word != '-') {
        ROOT.ep_square = (word[1] - '1') * 10 + word[0] - 'a' + A1;
    }

    strtok(0, " \n"); // halfmove clock

    strtok(0, " \n"); // fullmove counter
}
#endif
