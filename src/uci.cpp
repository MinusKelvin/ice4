
#ifdef OPENBENCH
void parse_fen();
#endif

int atosq(char *move) {
    return (move[1] - '1') * 10 + move[0] - 'a' + A1;
}

void uci() {
    setbuf(stdout, 0);
    char buf[4096], *move;
    int wtime, btime, hash, value;
    fgets(buf, 4096, stdin); // uci
    puts(
        "id name ice4\r\n"
        "id author MinusKelvin\n"
#ifdef OPENBENCH
        "option name Hash type spin default 8 min 1 max 99999\n"
        "option name Threads type spin default 1 min 1 max 999\n"
#endif
        "uciok"
    );
    for (;;) {
        fgets(buf, 4096, stdin);
        switch (*strtok(buf, " \n")) {
            case 'i': // isready
                puts("readyok");
                break;
            case 'q': // quit
                exit(0);
#ifdef OPENBENCH
            case 's': // setoption
                strtok(0, " \n"); // name
                hash = *strtok(0, " \n") == 'H';
                strtok(0, " \n"); // value
                value = atoi(strtok(0, " \n"));
                if (hash) {
                    TT = std::vector<TtEntry>(value * 65536);
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
                        atosq(move),
                        atosq(move+2),
                        move[4] == 'q' ? QUEEN :
                        move[4] == 'r' ? ROOK :
                        move[4] == 'b' ? BISHOP :
                        move[4] == 'n' ? KNIGHT : 0
                    );
                    if ((ROOT.board[mv.from] & 7) == PAWN || ROOT.board[mv.to]) {
                        PREHISTORY_LENGTH = 0;
                    }
                    ROOT.make_move(mv);
                }
                break;
            case 'g': // go
                strtok(0, " \n"); // wtime
                wtime = atoi(strtok(0, " \n"));
                strtok(0, " \n"); // btime
                btime = atoi(strtok(0, " \n"));
                double time_alotment = (ROOT.stm == WHITE ? wtime : btime) / 1000.0;
                Searcher s;
                s.iterative_deepening(time_alotment);
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
        ROOT.zobrist ^= ZOBRIST_STM;
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
        ROOT.ep_square = atosq(word);
    }

    strtok(0, " \n"); // halfmove clock

    strtok(0, " \n"); // fullmove counter
}
#endif
