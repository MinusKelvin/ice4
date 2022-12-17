#define NEURONS 8
int16_t FT[25][SQUARE_SPAN][8];
int16_t BIAS[NEURONS] = {169, 155, 20, 72, -50, 3, -7, -12};
int OUT_W[2*NEURONS] = {-65, -84, 91, 115, 59, -72, -45, 77, 64, 85, -92, -163, -59, 75, 46, -74};
int OUT_B = 6554;

void unpack(int neuron, double base, double scale, const char *data) {
    for (int i = 0; i < 768; i++) {
        int piece = i / 64 % 6 + 1;
        int color = i / 384 ? BLACK : WHITE;
        int rank = (i / 8 % 8) * 10;
        int file = i % 8;
        FT[piece | color][rank + file][neuron] = (data[i] - ' ') * scale + base;
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

uint64_t ZOBRIST_PIECES[25][SQUARE_SPAN];
uint64_t ZOBRIST_CASTLE_RIGHTS[4];
uint64_t ZOBRIST_STM;

void init_tables() {
    // Feature Transformer
    unpack(0, -218.8, 5.032, "LJMLLKMLHKIAADHGJJJDABGGIJHEBACDIIGBA?@AHHC?;:;@LJD>97;BMKJKLKMMHKGFEFHJGFEBABCEHDC>>>ADDB?;::<BDA><:;<ACD@=>=?>DAA?<@>?DD<;<6:BFIPJIJKLHKIGFGJMHFFDCDFHGFDA@BCFGDE@?@BDFGEDABAEIHFCCBEFHECBCBCE:8;>??;<89:;<<;;:::;;;<>::::99:<9898778:766666775234341478999877*+.221.,*+-./1//)+*+*+,-*('&%&(*(&%$!#%*&%#$#$%'&!\"! %#($$#\"$'#\"`_^YYYZ[^\\YVSTUV]ZVSOOOQ]YUQMKJL[ZVPKGFH_\\VLEDCEdaXKCAAEe_WLGBALKKJLJJMJb_hY[Z`]YXXNLHMMUQQKHHJMQOOJJKLOONLOMPLQPNORQRNQJJLKJMJMW\\^\\Y_`XVWVWWVYVVSVVWXVWVXYZ[\\YVWXZ[[[ZWWWYZZXXXVXWXYWXXTVWXWYWUZ[[]Y\\WTVTVWVURTWUVUWVUTUWVWXXWUVVWWXVVTUUUXWVRTQSUVXTTRPTWWWWVQ_\\]\\]__`___^_^__a^`_``__a_a`a`_\\a`a`__^\\a_a_]^\\]a__^]]]]b___]`^^urtuvz~~qrsuwtvsqprsyusrprsvxwuqqqrsrsrqppqrrppnooprrpoopoqrqqppJBEOXZTNGCOT]ZZWKMRX__]WOSVZ^`]YQTVY\\][YRSTXZ[YVOPQTVUSRNOOONQPM");
    unpack(1, -185.3, 5.983, "??@???@>:89<>?@=:89;>=?>979;=>>>979;=?@@9779=@CB977;BEGF>?@>>?@@<:89999;9678898987667898745567787555667877657798867669::>857789?>;8898:::86789:976567888755667787555677876666888987677:::876667:7754333375321221542221322211122200011111/0/00110-...//00/0111221/.*+((('-*(&&&%#*)%$$$$#)&#\"\"###'$\"!!\"#$'%\" !!\"$&$!  \"\"$&#\"\"!!!\"==>FJLPP::;CGKNO89;@FILN8:>BGJLO;<?BHLNP<=?BHLPQ==:@FOUXE>@EHORT@>?>?@??=><=KU~[678<BEKI99:=ABDE===?AADC?>?AA@BA??@ACABB>>@@=@>@FLMKHONMHHGHHGKGIHKIGGGGJHJHIHIGHIHHIHHIHHGHHHHHIHHHGGHGHHHGGGGFHJJIINKJFHIHIGIGJIIHGHHIHHHHIIJHGHHHIJJIHFHHHHJJGHHHHIIIHIIHIIJGMNNNOOPPMOOPPNOONOPOONNOLNNNOOOOLMMNOONOLLMMONNNLLLMNNNNNNMMNONMfgdcaa``hgdcb```gfgcaaabedcbbbacdcbbbbbbdcbaababdccbbbbbedcccdeeYYTSKKC@WbYSPHCBT[WRMHECQRQNKHFFLMLJHFFEHHGFDCCBFEDBAA@@DDC@??@>");
    unpack(2, -202.2, 4.938, "JHIHHJJISOPNOONPRNPMONOPROQOQOQQRQTRSPSQSRUSURVSSTZWZXdWHJHHGIJKSVRRQQSSRSSRSRQQRRSRRSRRQPRRRRQQPQPPRPRQRQOPPOPPQQPPOPQQPQQQPTQQZVXVWTXXXXUWUVUWXVWUVVVUVVVVUVUWWUUTUTVVTUUTTTTVYUWUWUXUVWVYWZVXjhgfgfhhjigffffgjhhgggggkihghhiikhgfffhiifedcbdggecccbdecccbb`ca|{{|yy}~{|{zzxz}|||{{{{{}}|}|||||}~{|{~}|}|}|}}||}~|~z|}}|{{zy{}GHJMOMHGGJKMMMKKJKMNOOONKLMNOPOPKJKKLLNMJIKKKJIKLILLNIKHINLMNJHFHJGIHJHJ;EGKEEA=:AIKJHB>?DHLKIEACEHJKIGEEFHHIIGGDEGFGGGFIHGJJIJIFDDFHCBDHGFHFGDGIHHIJIHFGFHHIIGFFEFFGGGEDCCEEDDDCDCCCCCDCEBAABDD<??A@>>>A@CBCAA?BBCEEECCDBCEFCDBCBBDCCBDCABAAABB@A@@@A@A@=A?@A@?;9:9::9:;8:::::;:788:9::9677889964545567544334554443345565534555&&&('*-s' #!\"\"%($!!!!%%&%!  #\"\"$%! ! !\"#$\"!   \"#$\"#\"\"\"\"$%$%'$$$%NK?<NKKMKNKROLKNNMMRPMKLKLMONMLKLLLMLMMMLLLKKLNNNMMLLNOPNNOMMNPO");
    unpack(3, -131.8, 2.953, "KNNNKLJMRUUPSSSPRTUPTQTPRRURUSTPUVXUYVXS[\\_]^\\\\X`]b`cX[TNPMKNOOJZW[[[ZYY[Z^\\]\\[ZZ]_`__^]^aaaaba_`bcdedcabfggiffaa`eheg__c^\\_cXY[aa``__`cdedbbbdbdeeedeeccdeggfddcdfhjggcdghhhkfeehgfgfieb`a_b]ac^`bbdceb^`babbc`bcbabbecbcdccdecbedeeffecedefgfedeegffdfghjjiiifnprsttussuwvvvwuvxxwwwxwwwxyyyxxxxx{zzzyy{||{{zyy|{{y{yxx|~~~{xva_[YUTUY\\[YVSSUTWWTRQPQPPSTSQONLNRSRSPNMQQSRTSQNUNVNSOTLWWSMTPSMKKMKJNLN/2/33<9?<97:;?>BCB@DBDCGGFEGFHGJHFFJGJFJHEEIHHFJPKNMOMJN0;@><BB<9:8775<<9545466:;:87689:=;:999<;?<::;;;<@?<=<=>@?A>>>?@?6>=?<@=7645876568442335887443557775454877656665765678768899::::63310011376667575:7766657;9877787::99998898999967<9898879:9765656#! !!!\"\"! !#%#%##!! $#$$$%$#$$$#$$$#$#%$$#$####$&%#####%*(&&%%$%SOTXPTSWQRPTQSOXUTSRSTW[XVUUVZZ]XWVXZ\\\\^TTWYZZYZMORTWVUTLLNQSUSP");
    unpack(4, -288.9, 4.775, "]]]\\^[[[`\\[ZZ\\jg^]\\ZX[eg`_\\ZXY`cb`^\\YWY\\eca\\XVX\\ee]XRTRW[^^[]]\\[^`__`bc]_a`ba_dbbbabababbccbbbbbbddcccdbcadcbba_bccddc`_`ehdddaY``abbgeWa_aaacfdbbbabcdgcdbcccecbdddcdcdddfdcccdadeecb\\\\ceggfeb_fhhhhdV>hhiihe_Bghiiigd]giijjhhegjijjihihijkljjljlllljkmgiijkhjnzzyyzxnB{zxwyxrkzzzyzyxsz}|{{zyw|~}}|{zw|{|{{|zv{||}}y~{}zzyy|||\\]]\\[eke_]]]^`fh]^^^_`cg]^^__abd^]]]^`bc]^^\\\\^aa[daZ[Y\\]Y_^[ZXS[^^[[[]\\^QPHKEZne]\\YYW]gj[\\[\\Z]ddYZ[[Z[_cYY[YYZacWYXWWZ_b]\\\\^][\\^[[USVTWXYYVUW^[UZXVXVRYXWVUTWUUVVUUUUVTWUVVUUVVWWWVVVVVTXUWWWVWYYWXVVURUX[XXWSWVVXWXXWRSWUVSVVWWWUTTUWWXVTTTUUWUUWTVUUVUUSUVVTTUKLLNNMQSJKJJJLMTJLKLMNOSJLKLMOOTMLLKMNQSMNMLLNPRKNMLMNPSKLMMOLPSFHHIIEDJIKJHHDFGIJKKJFDDHGGGGFFFGGFDFFDEGFDEEEEEEFEDFFDFACDDEEEFRVYVUM+*WZX[TG% XZ\\^[SJH][]_^[ZX^]___a`_^^``acdc`abdecbacceijgce");
    unpack(5, -428.1, 6.464, "cccccaacghfc`^]aggcaa`^aecbaabbbdaa`accdda``beefc]_^`deecbbbcbcadbba`__`caba_^_^aa``__^_ba```__^```__^^_a`__^]]^`a_^^]_^b_^^^__`c`a`a`b_bca`__^`baa_`^_^ab_`_`^`a_`_`_`_a_^`_`_`a___a`b`a_^``b`a_```__^_a`a^^^]^bb`___]]cba_^^^]cba_]]]]ca`^]\\]\\ba_^]\\\\\\b^_^]\\\\\\ZZ[\\]`b_[\\^][[[[_^\\[[ZZZ_]\\ZZ[[Z_][ZZZZZ`_]ZZYZ[__\\[ZZZZ_[[ZYWXY}|ynb`][zxukd`^\\wtpjda^^ilkhda```cddcbac][_bb`_aIU]_^^\\_C9@LZZQNabbaaaabE MW_bcdTQTZbfff[Z[_cfee___abcdcbbbbbccbbcbbaccbbacbdbacNV]bebei_abddeehabbdfgghbcdeehhidcddfghhdddeffggdeeeefgfedeeefefbcdgffghb`cdeeghbcbeffhiedddegfgefeeeefgfffeedeeghgededehgfefdfdkgggghghjhhiijijkhhiikjkigggijkjgffghjjjdefghjkiddefhikieffghhjivqswxxx{ursuz||~vusuy~~~wvuuy{{{wvvvwyz{vwwvwwxzvvwvwwyzxxwxxxz{YMVY`eidVQUY_eigUVY[afhiUXZ^bgij]]^aehijcbbcfghifeeeeghhgfffffgh");
    unpack(6, -584.9, 9.088, "``a``a`_b\\dXf[d]b\\fWgXd\\b[eWgXe\\b\\fXgVd\\c[eXeSd[h'h i'h+aa``a`a`^]^]^]^^^\\^]^\\^]]]^^]^^_^]]]]]^]^[_Z`\\_\\_[_[_[_\\\\^Z_\\_\\]\\]Z]Z\\Y^]]^\\]\\^]\\]\\_[^\\]]\\]\\]\\^\\\\][^\\_[^^[_Z`[_[Z]Z][^[^_Z`Y^Z_ZZ]Z\\Y\\Z^[Z\\Y\\Z]Z[Y\\Y]Y]Z[Z[Y\\Z\\Z[Y[Z\\Y\\Z[Y[Z\\Z[Z[Z[Z[Z[Z[Z[[[Z[Z]W\\Y\\X^XRRRRRQSQRQRRRQSPRRQRRRQQRRQRQSQQRQQRQRRQQQQQQRQPRPQQRQRPROQQSQQPba`bababcaaaabbbbbaaabbabaaabbbbaacafcdbd_ecjjh_yJxGyguVz{~x~zxEa```a`_``b`cbcbc`c`aabacab`c_bbcabaabacbabaabbbcabb`cacb`_aaa```acacab_`bebc`babeccbbcb`fbead`c`ebeaead`edddcccbcebebebdcecebdbd`fbdbcaeeaeadadacdcdbcbcecdaebdacdbdbebddbecdcecbecdcdcfebebecebfbfcebebgcgcfbfbfcfcfcfbgdfdfcecfdfeedededeeeedceeeefeddedeeeeddknllmllnoloknjnknnnlnllknmnlnlllnnmmmmmmnmnmnmmmmnnmnmmnmmnmnmnmjeieghfagfgeifhfgeeffefeeedddcccddbdacaaebcbbab`bbbabaa`cabab`a_");
    unpack(7, -298.5, 5.199, "[YXZYYXZ[[\\^__][[\\]^``_[[\\]^`_\\Z[ZYZ[XWVWQNNLLKPTLFFCEENYYYZYXZ[kljjkjjhjiiiiiihjihggggfgedeccadecb``a_ba`^_^^`^a_\\\\\\\\]ZQ]WYY<TIefehhhfeecfehffdbdceefdcdbbcdccbbcaa_aabbb``_]a_ecba^a]badc`aX_[onmljjhlqnmmkjhkomlkjiijonnlkkjknnmljijklljiggghggggeeeeefecbcdc~~}~||{{|||}||zz|{{|{zyx|{zzyxwv{zxwttttyyvvtqrqxvurprqsuusponhX[^`cdd_]\\_befea^^`ceedb_[]_bba`^YY[\\^^][UTRUYZXY #0/85.0?4<7:L6&YXXXZ[YZSW]ble\\SVWZ^a`\\ZUUVYY[YYUUVYXZXYTUVZX\\Y[VWY[[^\\]X[XZZZYZHCBDDA@HFFFHIGEGGFIKJJIJIKLMONOLHLMNOPNMIKLNNONNHIKLMNLLHGJKLMLJJIJGIEJLJJJJGIFLJJKJIILJHLJMMMLKIIMOPNMJILMOMONMJJMJMLMJIKIKJKKL;<<;;;=;?@@@B@BB@AABCBCDACCDFFHE@CDFGHGF@BCDEFGE>@ACDFFF=?ACCEFC%&')(&(''&((+$&('()*',*)()++/..+(*,/0/-+'*,----+&)**++,*$''*)*+*IQF= 3I5OJH)5):JXSQOIJNS_\\XXXXZYa`_aaba]bbdefedafddccccdhda`a`cd");

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
