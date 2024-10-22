#ifdef TUNABLE

map<string, variant<int*, float*>> tunableParams;

struct RegisterParam {
    RegisterParam(string name, variant<int*, float*> value) {
        tunableParams[name] = value;
    }
};

#define PARAM(NAME, TYPE, VALUE, MIN, MAX, STEP, LR) TYPE NAME = VALUE; \
    static RegisterParam registerParam##NAME(#NAME, &NAME);

#else
#define PARAM(NAME, TYPE, VALUE, MIN, MAX, STEP, LR) constexpr TYPE NAME = VALUE;
#endif
