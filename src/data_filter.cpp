#include "everything.cpp"

struct PackedBoard {
    uint64_t occupancy;
    uint8_t pieces[16];
    uint8_t stm_ep_square;
    uint8_t halfmove_clock;
    uint16_t fullmove_number;
    int16_t eval;
    uint8_t wdl;
    uint8_t extra;
};

static_assert(sizeof(PackedBoard) == 32);

void unpack(PackedBoard board) {
    ROOT = Board();
    int piece_idx = 0;
    int seen_king[2] = {0, 0};
    int remove_short[2] = {1, 1};
    int remove_long[2] = {1, 1};
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            if (board.occupancy & 1ull << (rank*8+file)) {
                int piece_code = (board.pieces[piece_idx / 2] >> 4 * (piece_idx % 2)) & 0b1111;
                piece_idx++;

                int piece = piece_code + 0b1001;
                if ((piece & 7) == 7) {
                    if (seen_king[(piece & WHITE) != 0]) {
                        remove_short[(piece & WHITE) != 0] = 0;
                    } else {
                        remove_long[(piece & WHITE) != 0] = 0;
                    }
                    piece = (piece & INVALID) | ROOK;
                }
                if ((piece & 7) == KING) {
                    seen_king[(piece & WHITE) != 0] = 1;
                }

                ROOT.edit(rank*10+file+A1, piece);
            } else {
                ROOT.edit(rank*10+file+A1, EMPTY);
            }
        }
    }
    ROOT.stm = (board.stm_ep_square & 0b10000000) ? WHITE : BLACK;
    if (ROOT.stm == BLACK) {
        ROOT.zobrist ^= ZOBRIST.stm;
    }
    for (int i : {0, 1}) {
        if (remove_short[i]) {
            ROOT.remove_castle_rights(i, SHORT_CASTLE);
        }
        if (remove_long[i]) {
            ROOT.remove_castle_rights(i, LONG_CASTLE);
        }
    }
}

int main(int argc, char *argv[]) {
    init_tables();

    ifstream input(argv[1], ios::binary);
    ofstream output(argv[2], ios::binary);

    PackedBoard board;
    int total = 0;
    int kept = 0;
    while (input.read((char *) &board, sizeof(board)).gcount() == 32) {
        unpack(board);

        Move mvs[256];
        Searcher s;
        memset(&s, 0, sizeof(s));
        s.abort_time = 1.0 / 0.0;
        s.conthist_stack[0] = &s.conthist[0][0];
        s.conthist_stack[1] = &s.conthist[0][0];

        int count;

        ROOT.movegen(mvs, count, 0, s.mobilities[3]);
        ROOT.null_move();
        ROOT.movegen(mvs, count, 0, s.mobilities[2]);
        ROOT.null_move();

        s.negamax(ROOT, mvs[0], LOST, WON, 1, 2);

        if (!ROOT.board[mvs[0].to]) {
            output.write((char *) &board, sizeof(board));
            kept++;
        }
        total++;

        if ((total & 0xFF) == 0) {
            printf("\r%d / %d", kept, total);
            fflush(stdout);
        }
    }
    printf("\n");
}
