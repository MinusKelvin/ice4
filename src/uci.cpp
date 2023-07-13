
#ifdef OPENBENCH
void parse_fen();
#endif

void uci() {
    vector<TtEntry> tt(HASH_SIZE);
    setbuf(stdout, 0);
    char buf[4096], *move;
    int wtime, btime;
    int prehistory_len;
#ifdef OPENBENCH
    int opt, value;
#else
    int firstmove = 1;
#endif
    uint64_t prehistory[256];
    fgets(buf, 4096, stdin); // uci
    printf(
#ifdef OPENBENCH
        "id name ice4 v3.1\r\n"
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
                uint64_t nnue_idx;
                fread(&nnue_idx, sizeof(nnue_idx), 1, RNG_FILE);
                QNNUE = VARIANTS[nnue_idx % (sizeof(VARIANTS) / sizeof(QuantizedNnue))];
                break;
            case 's': // setoption
                strtok(0, " \n"); // name
                opt = *strtok(0, " \n");
                strtok(0, " \n"); // value
                value = atoi(strtok(0, " \n"));
                switch (opt) {
                    case 'H':
                        tt = vector<TtEntry>(value * 65536);
                        break;
                    case 'T':
                        THREADS = value;
                        break;
                }
                break;
            case 't':
                train();
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
                prehistory_len = 0;
                while (move = strtok(0, " \n")) {
                    Move mv(
                        move[0] + move[1] * 10 - 566,
                        move[2] + move[3] * 10 - 566,
                        // maps promotion chars to piece enums
                        //       'q'    'r'    'b'    'n'    '\0' 
                        // cast  113    114    98     110    0
                        // % 53  7      8      45     4      0
                        // * 5   35     40     225    20     0
                        // % 6   5      4      3      2      0
                        // enum  QUEEN  ROOK   BISHOP KNIGHT EMPTY
                        move[4] % 53 * 5 % 6
                    );
                    prehistory[prehistory_len++] = ROOT.zobrist;
                    if ((ROOT.board[mv.from] & 7) == PAWN || ROOT.board[mv.to]) {
                        prehistory_len = 0;
                    }
                    ROOT.make_move(mv);
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
                double time_alotment = (ROOT.stm == WHITE ? wtime : btime) / 1000.0;
#ifdef OPENBENCH
#else
                if (firstmove) {
                    double n = now();
                    train();
                    time_alotment -= now() - n;
                    firstmove = 0;
                }
#endif
                ABORT = 0;
                FINISHED_DEPTH = 0;
                vector<thread> threads;
                for (int i = 0; i < THREADS; i++) {
                    threads.emplace_back([&]() {
                        Searcher s;
                        s.tt_ptr = &tt;
                        s.prehistory = prehistory;
                        s.prehistory_len = prehistory_len;
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
                for (auto& t : threads) {
                    t.join();
                }
                printf("bestmove ");
                BEST_MOVE.put();
                putchar('\n');
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
