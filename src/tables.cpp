int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-8, -14, 12, 18, 14, 11, -8, -5};
int16_t DOUBLED_EG[8] = {40, 23, 15, 7, 8, 18, 24, 43};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 51
#define TEMPO_MG 6
#define TEMPO_EG 3
#define ISOLATED_PAWN_MG 12
#define ISOLATED_PAWN_EG 15

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
            white_section[rank+file] = v;
            white_section[7+rank-file] = v;
            black_section[70-rank+file] = -v;
            black_section[77-rank-file] = -v;
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
    unpack_full(0, PAWN, "        >Y[>:;3&DZOHDC6-@KTRPG4,CR`_ZQB4L[urmaK9C \\~q`1^        ", 1.156, 11); // average: 49
    unpack_half(0, KNIGHT, "EHJLILSPLUVYW\\\\\\_]hf_sy~XWqu 2'a", 1.832, 140); // average: 241
    unpack_half(0, BISHOP, "LJHHV[XRV`][]Z_g[^kr`rv~V^b\\B4 2", 1.252, 172); // average: 242
    unpack_half(0, ROOK, ".68A .24+70136:<CIPVQchsa]v{wx~~", 1.219, 264); // average: 318
    unpack_half(0, QUEEN, " )(,.35256514321C4::FPHLI6K;=Sni", 1.0, 667); // average: 695
    unpack_full(0, KING, "::6.62BF94.&)19>),  !$.+%*(*'') +1/400,#452762*,G/:?:1#4~dc]UQUa", 2.874, -58); // average: 4
    unpack_full(0, PASSED_PAWN, "        5C651/.715&$ \"/40.#\"'*:<04.157HC<,64=IS[DD;7Rb~U        ", 1.222, -18); // average: 5
    unpack_full(1, PAWN, "        &'151/).\" +*(%&%%((%$'+).0-)+.33;G:?>?A?`Joz{~p}        ", 1.423, 120); // average: 147
    unpack_half(1, KNIGHT, "'5JNDVY`M`gp[ivx_rx~YgqqM_dk esa", 1.477, 347); // average: 438
    unpack_half(1, BISHOP, " 2$/7;@?;HQT7JW[9TS^>MUM7RQRNV[W", 1.0, 453); // average: 492
    unpack_half(1, ROOK, " ...*).1/3:;?EFDJPMIQJKDPVPPFLKK", 1.0, 767); // average: 799
    unpack_half(1, QUEEN, "6#! 2./3:CJHKU]eNinuUfuvTqq~]_SW", 1.66, 1486); // average: 1568
    unpack_full(1, KING, "+./*$,+ 1:@ED?6/;DMPPKB<DNTWVSLEM[_^`^\\NXloiion[Nztmos~S&SW[\\[Z(", 1.945, -84); // average: 6
    unpack_full(1, PASSED_PAWN, "        ## \"%'.%'+'&&'+'6:30,-24EF>6249?UTN<7<FNg~^C8:OJ        ", 2.66, -17); // average: 32

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
    auto rng = fopen("/dev/urandom", "r");
    fread(ZOBRIST_PIECES, sizeof(ZOBRIST_PIECES), 1, rng);
    fread(ZOBRIST_CASTLE_RIGHTS, sizeof(ZOBRIST_CASTLE_RIGHTS), 1, rng);
    fread(&ZOBRIST_STM, sizeof(ZOBRIST_STM), 1, rng);
#endif
}
