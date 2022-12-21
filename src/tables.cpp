int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};

void unpack_full(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 8; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = v;
            black_section[70-rank+file] = -v;
        }
    }
}

void unpack_half(int phase, int piece, const char *data, double scale, int offset) {
    int16_t *white_section = PST[phase][piece | WHITE];
    int16_t *black_section = PST[phase][piece | BLACK];
    for (int rank = 0; rank < 80; rank+=10) {
        for (int file = 0; file < 4; file++) {
            int v = (*data++ - ' ') * scale + offset;
            white_section[rank+file] = white_section[7+rank-file] = v;
            black_section[70-rank+file] = black_section[77-rank-file] = -v;
        }
    }
}

#ifdef OPENBENCH
// Deterministic PRNG for openbench build consistency
uint32_t rng_32() {
    static uint64_t RNG_STATE = 0xcafef00dd15ea5e5;
    // Pcg32
    uint64_t old = RNG_STATE;
    RNG_STATE = old * 6364136223846793005ull + 0xa02bdbf7bb3c0a7ull;
    uint32_t xorshifted = ((old >> 18u) ^ old) >> 27u;
    uint32_t rot = old >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
uint64_t rng() {
    return (uint64_t) rng_32() << 32 | rng_32();
}
#endif

uint64_t ZOBRIST_PIECES[23][SQUARE_SPAN];
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;


void init_tables() {
    // Piece-square tables
    unpack_full(0, PAWN, "        =g]=9:>#BhOHCBA*>XUTQF@)A_`^XNN1Kgusn^V6> Z~q_5\\        ", 1.047, 15); // average: 51
    unpack_half(0, KNIGHT, "EHJLIKRPLUVYV\\\\\\^]hf^sx~XWqu 1(a", 1.817, 146); // average: 245
    unpack_half(0, BISHOP, "JHGFSZVPT^[Y[Y]eY]jq_rv~U]b\\B3 1", 1.214, 178); // average: 244
    unpack_half(0, ROOK, "/69A .34+6/1269<BHPVObgs`\\w|xx}~", 1.203, 269); // average: 323
    unpack_half(0, QUEEN, " )(--35245403320C4:9DPGKH5K;>Qlh", 1.0, 675); // average: 703
    unpack_full(0, KING, ";<8083DG:5/&)2:@*-  !$/+&+(*'(*!,30511/$662883-/G0:>;1#5~cg_VQT[", 2.738, -60); // average: 2
    unpack_full(0, KING+1, "        /?762.(1-0&& !(.,*$\"&)37,0/147B?9+96>JOXEI?>Ud~T        ", 1.226, -19); // average: 2
    unpack_full(1, PAWN, "        $0272/1, )*,)%.#$/(%#(2'+4+)*-607K8@>=D;_Em~|zm{        ", 1.342, 120); // average: 146
    unpack_half(1, KNIGHT, "'6JMEWY`M`gp\\iwx_sx~YgqpO_cj dsa", 1.459, 337); // average: 427
    unpack_half(1, BISHOP, " 2$07<A@=IQT8KW[;US^?MTM:RPQOU[V", 1.0, 440); // average: 479
    unpack_half(1, ROOK, " -..*(-1.29:=CEBINKGOHIBNTMNCJIH", 1.0, 750); // average: 781
    unpack_half(1, QUEEN, "7$  4//4;DJHLV^fOjnvVeuvUrq~^_TW", 1.615, 1456); // average: 1536
    unpack_full(1, KING, "+/0+%-, 2;BFE@70=FORRLC=EOUYXTMFN\\__a^\\OXloiionZOztmns~R\"SVZZYY&", 1.871, -83); // average: 5
    unpack_full(1, KING+1, "        \"  \"%&)#')'&&(('5740--/2DD?7357>UQN<8=CMf~^A8<OI        ", 2.647, -25); // average: 22

    // Zobrist keys
#ifdef OPENBENCH
    for (int i = 0; i < 23; i++) {
        for (int j = 0; j < SQUARE_SPAN; j++) {
            ZOBRIST_PIECES[i][j] = rng();
        }
    }
    ZOBRIST_CASTLE_RIGHTS[0] = rng();
    ZOBRIST_CASTLE_RIGHTS[1] = rng();
    ZOBRIST_CASTLE_RIGHTS[2] = rng();
    ZOBRIST_CASTLE_RIGHTS[3] = rng();
    ZOBRIST_STM = rng();
#else
    auto rng = fopen("/dev/random", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
