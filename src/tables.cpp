int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[8] = {-3, -13, 13, 19, 15, 12, -7, -1};
int16_t DOUBLED_EG[8] = {29, 17, 10, 3, 5, 11, 17, 32};
int16_t VIRTUAL_QUEEN_MOB_MG[] = {8, 13, 12, 10, 10, 10, 8, 6, 3, 0, -6, -11, -18, -22, -28, -33, -33, -31, -23, -15, -5, 2, 14, 31, 4, 67, -10, 21};
int16_t VIRTUAL_QUEEN_MOB_EG[] = {7, 22, 21, 20, 17, 14, 14, 16, 16, 18, 20, 21, 22, 21, 21, 18, 14, 8, 1, -7, -16, -24, -31, -43, -45, -64, -66, -75};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 21
#define BISHOP_PAIR_EG 43
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 11
#define ISOLATED_PAWN_EG 14

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
    unpack_full(0, PAWN, "        @\\^A?@7)H^VNKI<2DQ\\ZXN;2IZigbXI;Ud}ysgR@D g~rl0e        ", 1.032, 11); // average: 49
    unpack_full(1, PAWN, "        (&/532,2% *,*())(((&&*.-/.,),046:E:>??A@_In~{wuv        ", 1.274, 91); // average: 116
    unpack_full(0, PASSED_PAWN, "        2=420-.613'' \"/432(%')9<171135EA:.:4<EOVNMACWY~S        ", 1.274, -18); // average: 6
    unpack_full(1, PASSED_PAWN, "        $$ \"$%-$','%&(+(6:30,.34EG>725:?VTN<7=GMd~Y928FF        ", 2.037, -10); // average: 27
    unpack_full(0, KING, ">?:2:5FI=92),5=A.1##%'2//2)$ *1(7;/(!/72F<-2-/5?R=>=93*C~dshbQYS", 2.361, -58); // average: 3
    unpack_full(1, KING, "9@DA;C>1BMTZYSJ@KVbff`TKO]issh[PUgr{}rfSYt~z|~sZMyxuuw{N UUZ[XW(", 1.865, -111); // average: 0
    unpack_half(0, QUEEN, " )),,3514440312/@177?KBHA0F82Iad", 1.0, 635); // average: 660
    unpack_half(1, QUEEN, ":& !5-,4;DJHLW^gOjqwXevvWtq~ebWT", 1.402, 1256); // average: 1326
    unpack_half(0, ROOK, "/58A .45*60337<?CJSZQejw`\\x~uw}~", 1.098, 289); // average: 339
    unpack_half(1, ROOK, " +)'(&'*,.329<<9ADA=E>>9DJBC>B@@", 1.0, 627); // average: 650
    unpack_half(0, BISHOP, "HFEESYUPR]ZXZX]eX\\jp^qv~RZa\\6/ 3", 1.098, 196); // average: 255
    unpack_half(1, BISHOP, " 1&.46;;:CIJ7FOP:NLT=IMG:LJMQSWR", 1.0, 360); // average: 395
    unpack_half(0, KNIGHT, "ILNQNPVTPYY\\Y^__``ig`tx~ZZqt 5,e", 1.855, 146); // average: 252
    unpack_half(1, KNIGHT, "&6HLETW^M_fm[huw^rw~XdooL_ai dt_", 1.231, 273); // average: 348
    
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
