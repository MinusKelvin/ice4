int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-7, -14, 13, 18, 15, 12, -8, -4};
int16_t DOUBLED_EG[8] = {39, 23, 14, 6, 8, 17, 23, 42};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 50
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
    unpack_full(0, PAWN, "        ?Y\\?;<4'E[PJFE7/AMVTRH6.ETb`[RD5N]wsobM;D _~qc0_        ", 1.168, 11); // average: 51
    unpack_full(1, PAWN, "        &'161/)/\" ++)%&%%((%$'+).0-)+.33;G:?>?A?aJp||}r~        ", 1.412, 118); // average: 144
    unpack_full(0, PASSED_PAWN, "        5B641/.724&% \"/41/$\"'*:<05/157HC<-74>HRZFF<:S`~U        ", 1.272, -19); // average: 5
    unpack_full(1, PASSED_PAWN, "        ## \"%&.%'+'&&(+'6:30,-24EF>6249?UTN<7<FNg~]A7:MI        ", 2.607, -15); // average: 32
    unpack_full(0, KING, ";;7/63CG:5.&)29?*-  !$.+%+(*'(* +20400-$652873,.G1:?:1#5~cf^VPU^", 2.813, -59); // average: 3
    unpack_full(1, KING, "+/0+%-+ 2;AFE@70=ENRQLC=EOUXXTMFN\\`_a_\\OXmoiioo[Nzunos~R#TV[\\[Z&", 1.908, -83); // average: 6
    unpack_half(0, QUEEN, " ))-.46356514321C4::EPGLH5K;;Qli", 1.0, 686); // average: 714
    unpack_half(1, QUEEN, "6#! 2-.3:CJHKV]eNinvVeuuTqp~__TV", 1.653, 1474); // average: 1555
    unpack_half(0, ROOK, ".69A .34+70136:=CIQWQdhta\\v{wx}~", 1.225, 275); // average: 330
    unpack_half(1, ROOK, " ..-*)-1/3:;?DECJOLHPIJCOUNOEKJJ", 1.0, 759); // average: 790
    unpack_half(0, BISHOP, "KIHGV[XRV_\\Z\\Z^g[^kq`rv~U]b\\@3 2", 1.26, 180); // average: 250
    unpack_half(1, BISHOP, " 2%07<A@<IQT8KW[;US^?NUM9RQRPW\\X", 1.0, 446); // average: 486
    unpack_half(0, KNIGHT, "FHKMJLSPLVVZW\\]]_^hf_sy~XXpu 2(a", 1.881, 145); // average: 249
    unpack_half(1, KNIGHT, "(6KNEWY`M`hp[ivx_rx~YgqpM_cj esa", 1.477, 340); // average: 432
    
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
