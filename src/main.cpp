#include "everything.cpp"

#ifdef OPENBENCH

#include <ctime>

int64_t perft(Board& board, int depth) {
    Board mkmove;
    Move moves[256];
    int mvcount;

    if (board.movegen(moves, mvcount)) {
        if (depth == 0) {
            return 1;
        }
        int64_t count = 0;
        for (int i = 0; i < mvcount; i++) {
            mkmove = board;
            mkmove.make_move(moves[i]);
            count += perft(mkmove, depth - 1);
        }
        return count;
    } else {
        return 0;
    }
}

#define BENCH_FENS "skipped\n"\
    "r4rk1/5pb1/3R2p1/p2Q1qBp/8/7P/1P3PP1/2R3K1 w - - 4 29\n"\
    "r2qkbnr/ppp2p2/2npb3/4p1p1/2P1P2p/1PN1N3/P2PBPPP/R1BQK2R w KQq - 0 10\n"\
    "3k4/8/4Q3/P2P4/8/5K1P/8/8 b - - 14 68\n"\
    "1R6/2Pb1p2/6p1/2K2k2/7Q/8/5P1p/8 w - - 4 58\n"\
    "r1bqk1r1/ppp1pp1p/2np2p1/7n/3PP2P/1PP1P3/PB1N2P1/R2QKB1R w KQq - 1 13\n"\
    "3rr1k1/2p4p/1p1n2p1/nP1P1p1P/8/2B2PR1/P7/1K1R1B2 b - - 0 33\n"\
    "5b2/2p2P1r/4k3/2P1P3/3B3p/7R/2K5/8 w - - 1 42\n"\
    "8/p3rk1p/P1K1p1p1/3p4/8/r7/8/8 b - - 5 47\n"\
    "rn1qk2r/pbp2ppp/3p1n2/1P2B3/1b6/2N2PP1/2PPP1BP/R2QK1NR w KQ - 0 10\n"\
    "1nb1kbnr/rpp1qppp/8/pP1pp3/P6P/5P2/2PPP1P1/RNBQKBNR w KQk d6 0 6\n"\
    "8/5pk1/6p1/2r5/4K1P1/8/8/8 b - g3 0 49\n"\
    "r1bqkbr1/p2np2p/8/3p1p2/RP5Q/5N1P/2PP1PP1/2B1KB1R w K - 2 14\n"\
    "r1b2rk1/1qnnbpp1/1p2p2p/p2pP3/3P4/P1NB1N2/1BQ2PPP/R5KR b - - 1 17\n"\
    "5k2/5p2/2p2b1P/4rN2/1P1p1R2/3K2P1/P1P5/8 w - - 1 39\n"\
    "8/8/1pbkp2p/2p1R3/2Pp1r2/pP1B3P/P7/4R1K1 b - - 1 37\n"\
    "8/5pkp/5np1/4B3/3p1P1P/1P4P1/2n2PBK/8 b - - 0 37\n"\
    "r2qkbnr/p1pp1pp1/4p2p/1P4P1/pn2P3/5N2/1BPP1P1P/RN1QK2R w KQkq - 1 10\n"\
    "2krn1r1/p1p2qp1/Pp5p/4P2P/3p2Pb/2N1P3/1PPBB1K1/R2Q4 w - - 0 26\n"\
    "r7/1p1b2k1/2np4/pB1p3p/P2P4/2PQPRp1/6P1/R4K1q w - - 3 30\n"\
    "r2qk1r1/3pb1pp/p1b1pp2/2p1n3/Q3P3/2N3BP/PPP1BPP1/1R1R2K1 w q - 2 18\n"\
    "2k2r2/1pp2Pq1/2n5/rN2p3/3pP3/PQ1P4/5R2/4R2K b - - 11 40\n"\
    "6rk/2p4p/3p4/3Pp3/8/PB6/P7/3KQ1q1 b - - 4 36\n"\
    "2r5/5pN1/3kpP1p/p2pn2P/Pp3R2/1P4r1/1KP1R1P1/8 b - - 2 45\n"\
    "rn1qkbnr/2pppppp/b7/1p6/p4PP1/4PN1P/PPPP4/RNBQKB1R b KQkq - 2 5\n"\
    "8/5pbk/7p/p1p2qp1/2P5/1P2B1P1/P6P/3RN1K1 b - - 2 32\n"\
    "2kr3r/1pp2p1p/p1n5/5qPR/3N2P1/2P5/1P6/2KR4 w - - 0 25\n"\
    "8/2k5/1p6/2r3p1/p2KB1n1/6P1/P4P2/1R3R2 b - - 1 43\n"\
    "8/p7/5Np1/PP1kp3/5n1p/7P/5KP1/8 b - - 4 40\n"\
    "r3kbnr/1pp1q1p1/2np2b1/pN2p2N/2P4p/P7/1P1PBPPP/R1BQ1RK1 b kq - 1 11\n"\
    "r2qr1k1/7p/1p1p1ppP/pPpPnb2/3p4/P5R1/1BPPQPP1/2KR1B2 w - - 0 19\n"\
    "rnbqkb1r/1p3p1p/p1pp1np1/4P3/4P3/N1PB3P/PP3PP1/R1BQK1NR b KQkq - 0 7\n"\
    "4R3/6pk/5p1p/4p2P/8/4PKP1/2r2P2/8 b - - 3 41"

int main(int argc, char *argv[]) {
    init_pst();
    if (argc == 2 && !strcmp(argv[1], "bench")) {
        char fens[] = BENCH_FENS;
        strtok(fens, " \n");
        uint64_t nodes = 0;
        timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (int i = 0; i < 32; i++) {
            ROOT = Board();
            parse_fen();
            Searcher s;
            s.iterative_deepening(1.0/0.0, 6);
            nodes += s.nodes;
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        double start_s = start.tv_sec + start.tv_nsec / 1000000000.0;
        double end_s = end.tv_sec + end.tv_nsec / 1000000000.0;
        printf("%ld nodes %d nps\n", nodes, (int) (nodes / (end_s - start_s)));
        return 0;
    }
    if (argc == 2 && !strcmp(argv[1], "perft")) {
        char buf[4096];
        buf[0] = 'a';
        buf[1] = ' ';
        fgets(buf+2, 4094, stdin);
        strtok(buf, " \n");
        parse_fen();
        printf("%ld\n", perft(ROOT, 5));
        return 0;
    }
    uci();
}

#else

int main() {
    init_pst();
    uci();
}

#endif
