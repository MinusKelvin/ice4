struct TtEntry {
    uint64_t hash;
    int16_t eval;
    Move mv;
    uint8_t depth;
    uint8_t bound;

    TtEntry() : hash(~0), eval(0) {}
};

struct TranspositionTable {
    std::vector<TtEntry> entries;

    TranspositionTable(uint64_t mb=8) : entries(mb*65536) {}

    TtEntry& spot(uint64_t hash) {
        return entries[hash % entries.size()];
    }
} TT;
