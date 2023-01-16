int16_t PST[2][25][SQUARE_SPAN];
int16_t DOUBLED_MG[] = {8, -8, 15, 17, 19, 14, -4, 7};
int16_t DOUBLED_EG[] = {30, 22, 15, 10, 8, 17, 21, 35};
int16_t PROTECTED_PAWN_MG[] = {0, 7, 9};
int16_t PROTECTED_PAWN_EG[] = {0, 7, 6};
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
#define BISHOP_PAIR_MG 22
#define BISHOP_PAIR_EG 42
#define TEMPO_MG 6
#define TEMPO_EG 2
#define ISOLATED_PAWN_MG 8
#define ISOLATED_PAWN_EG 11
#define ROOK_OPEN_MG 27
#define ROOK_OPEN_EG 11
#define ROOK_SEMIOPEN_MG 16
#define ROOK_SEMIOPEN_EG 16
#define KING_OPEN_MG -31
#define KING_OPEN_EG -7
#define KING_SEMIOPEN_MG -11
#define KING_SEMIOPEN_EG 18

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
    unpack_full(0, PAWN, "        FZ_?>?92IVPEDC77HOZVTK<:OWfb^UIC[b}vqfTJC f~tk5h        ", 1.085, 11); // average: 51
    unpack_full(1, PAWN, "        $(/721+.  *,+'&$#(&%$'+(+.+),.115E8>=<>:`Mn~zvvz        ", 1.302, 95); // average: 119
    unpack_full(0, PASSED_PAWN, "        /932.**0-/%( !,/0.%#%'67.50015C>6+74<EMRUMDGW[~V        ", 1.252, -15); // average: 7
    unpack_full(1, PASSED_PAWN, "        $#  $$,#','$%&*'7:40,-24FG>6149@XUO;6<GPd~Y5/6CC        ", 1.956, -6); // average: 28
    unpack_full(0, KING, "??:194FJ>91%(3=D,.  \"$/.*.)+'),#/835.23)<83;7414I9<>;1%9~`pf`MUS", 2.217, -55); // average: -1
    unpack_full(1, KING, "4:<;4<8+=GMUTMD;GQZ_^WOHOZaed_WOXeijlhdV^tvpqus_S|yssw~S ZY]][\\(", 1.706, -91); // average: 3
    unpack_half(0, QUEEN, " (*,+3522330101.=/55=H?E8,A4+BZ^", 1.0, 642); // average: 664
    unpack_half(1, QUEEN, "7& \"2*)08AHEJU[fKiowTcuuWro~ebVS", 1.335, 1265); // average: 1330
    unpack_half(0, ROOK, "149> +1/+4-,/054=BJNH[_iPNkpfkpo", 1.0, 287); // average: 325
    unpack_half(1, ROOK, " &%\"\"!#''*0/5::7=B?:B=<6CH@@@B@?", 1.0, 623); // average: 644
    unpack_half(0, BISHOP, "MJIIV]ZTT`][\\[_hZ^kq`rw~Q\\`\\:/ 3", 1.137, 193); // average: 256
    unpack_half(1, BISHOP, " 2*147==:DJN6GPR9NMU<HLF:JIKPSVP", 1.0, 363); // average: 398
    unpack_half(0, KNIGHT, "JLOQOQWUPZZ]Y_``a`jh_sx~WYot 3)b", 1.824, 149); // average: 253
    unpack_half(1, KNIGHT, ");MQHW[bObiq\\iwx^rw~XdnnM^ah et_", 1.239, 274); // average: 351
    
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
