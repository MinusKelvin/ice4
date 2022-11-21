struct TtEntry {
    uint64_t hash;
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;

    TtEntry() : hash(0) {}
};

struct TranspositionTable {
    uint64_t count;
    TtEntry *entries;

    TranspositionTable(int size=8) {
        count = size*65536ull;
        entries = new TtEntry[count];
    }

    ~TranspositionTable() {
        delete[] entries;
    }

    TtEntry& spot(uint64_t hash) {
        return entries[hash % count];
    }
} TT;
