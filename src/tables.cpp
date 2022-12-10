int16_t PST[2][25][SQUARE_SPAN];
int PHASE[] = {0, 0, 1, 1, 2, 4, 0};
int MG_OUTPOST_BONUS[] = {0, 0, 30, 20, 0, 0, 0};
int EG_OUTPOST_BONUS[] = {0, 0, 0, 0, 0, 0, 0};

void unpack(int phase, int piece, const char *data, double scale, int offset) {
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
unpack(0, PAWN, "$!!&'$ !3EA=?NQ88FEBFFS<7DGKMKJ9<LJQSRO;AUYec`TAtlr~t[5N# &%'!!'", 1.602, -6); // average: 50
unpack(0, KNIGHT, "FHKKLHIDFGNNORLJJRSWVUTMRWXXZ[[WXXac`f]^Yiks~zp]QTfrktU\\ /8Pl\"96", 2.074, 135); // average: 244
unpack(0, BISHOP, "OPMLIKJOZ[XRUWZSW_\\\\Y\\]W\\Z_dd][\\X[gnlj_]Tjhxx~rfWY^[`ed[C;C1G AP", 1.538, 177); // average: 265
unpack(0, ROOK, "-4<?C9:4!*03421 )0-.31:./17:=:<8<BIRVRPHIY[kwqoYXWm{wx_hqsy}|}|~", 1.258, 267); // average: 322
unpack(0, QUEEN, "\")+-)%, .0513891040005;92//-345:9/45<>9E8B:HL[VOB*;9<UDN3I`dl~XJ", 1.021, 671); // average: 699
unpack(0, KING, "BGC5;6JLD=4++4=C02%#\"%/-+-+)''*!/4/5/0.$550841*,D06<5. 4~ejbSQR[", 2.548, -68); // average: -7
unpack(1, PAWN, "!#\"#\"!\" IJIJIIJGFHDEEFGDHJECDEJFOOKHHIML_a[UVW`Zy}vlmv~y#! # ! #", 3.803, -5); // average: 142
unpack(1, KNIGHT, "*3GMMJ:'@XX__VUBJ]enne`NZguuwuh[]ovz~xtb[gstloh\\O`fjla`O fpf]td&", 1.519, 326); // average: 419
unpack(1, BISHOP, "*6 6635\"9?IEEAA2@OWXWRH@=O\\^\\YN9>[T_aZ[CIQZORUWD:WSYRSQ<UUX^Z`SI", 1.0, 431); // average: 474
unpack(1, ROOK, "/:::553 37==;3,,9@EEA?40JQROJJD>V^\\VQRPL^Y[SKNKP\\b]ZYQXPNSQQQOMH", 1.0, 727); // average: 767
unpack(1, QUEEN, "@2*'*\" 58=5:6-%7@DOILJD;EW[ja_WSLclrvpqZReptvwpbOotz~oua\\`YYWSdd", 1.795, 1420); // average: 1513
unpack(1, KING, "+./*%-+ 08>BB=6/:BIMMIA;CLRUTRMEMZ][]]\\QYjlegmo^Svqikr~[-UWWZ]`4", 2.156, -90); // average: 9

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
