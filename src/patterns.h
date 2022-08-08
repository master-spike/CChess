#include <stdint.h>

const uint64_t bishop_table[64] = {9241421688590303744ULL, 36099303471056128ULL, 141012904249856ULL,
                                   550848566272ULL, 6480472064ULL, 1108177604608ULL, 283691315142656ULL,
                                   72624976668147712ULL, 4620710844295151618ULL, 9241421688590368773ULL,
                                   36099303487963146ULL, 141017232965652ULL, 1659000848424ULL,
                                   283693466779728ULL, 72624976676520096ULL, 145249953336262720ULL,
                                   2310355422147510788ULL, 4620710844311799048ULL, 9241421692918565393ULL,
                                   36100411639206946ULL, 424704217196612ULL, 72625527495610504ULL,
                                   145249955479592976ULL, 290499906664153120ULL, 1155177711057110024ULL,
                                   2310355426409252880ULL, 4620711952330133792ULL, 9241705379636978241ULL,
                                   108724279602332802ULL, 145390965166737412ULL, 290500455356698632ULL,
                                   580999811184992272ULL, 577588851267340304ULL, 1155178802063085600ULL,
                                   2310639079102947392ULL, 4693335752243822976ULL, 9386671504487645697ULL,
                                   326598935265674242ULL, 581140276476643332ULL, 1161999073681608712ULL,
                                   288793334762704928ULL, 577868148797087808ULL, 1227793891648880768ULL,
                                   2455587783297826816ULL, 4911175566595588352ULL, 9822351133174399489ULL,
                                   1197958188344280066ULL, 2323857683139004420ULL, 144117404414255168ULL,
                                   360293502378066048ULL, 720587009051099136ULL, 1441174018118909952ULL,
                                   2882348036221108224ULL, 5764696068147249408ULL, 11529391036782871041ULL,
                                   4611756524879479810ULL, 567382630219904ULL, 1416240237150208ULL,
                                   2833579985862656ULL, 5667164249915392ULL, 11334324221640704ULL,
                                   22667548931719168ULL, 45053622886727936ULL, 18049651735527937ULL};

const uint64_t knight_table[64] = {132096ULL, 329728ULL, 659712ULL, 1319424ULL, 2638848ULL, 5277696ULL,
                                   10489856ULL, 4202496ULL, 33816580ULL, 84410376ULL, 168886289ULL,
                                   337772578ULL, 675545156ULL, 1351090312ULL, 2685403152ULL, 1075839008ULL,
                                   8657044482ULL, 21609056261ULL, 43234889994ULL, 86469779988ULL,
                                   172939559976ULL, 345879119952ULL, 687463207072ULL, 275414786112ULL,
                                   2216203387392ULL, 5531918402816ULL, 11068131838464ULL, 22136263676928ULL,
                                   44272527353856ULL, 88545054707712ULL, 175990581010432ULL, 70506185244672ULL,
                                   567348067172352ULL, 1416171111120896ULL, 2833441750646784ULL, 5666883501293568ULL,
                                   11333767002587136ULL, 22667534005174272ULL, 45053588738670592ULL,
                                   18049583422636032ULL, 145241105196122112ULL, 362539804446949376ULL,
                                   725361088165576704ULL, 1450722176331153408ULL, 2901444352662306816ULL,
                                   5802888705324613632ULL, 11533718717099671552ULL, 4620693356194824192ULL,
                                   288234782788157440ULL, 576469569871282176ULL, 1224997833292120064ULL,
                                   2449995666584240128ULL, 4899991333168480256ULL, 9799982666336960512ULL,
                                   1152939783987658752ULL, 2305878468463689728ULL, 1128098930098176ULL,
                                   2257297371824128ULL, 4796069720358912ULL, 9592139440717824ULL, 19184278881435648ULL,
                                   38368557762871296ULL, 4679521487814656ULL, 9077567998918656ULL};

const uint64_t rook_table[64] = {72340172838076926ULL, 144680345676153597ULL, 289360691352306939ULL, 578721382704613623ULL,
                                 1157442765409226991ULL, 2314885530818453727ULL, 4629771061636907199ULL, 9259542123273814143ULL,
                                 72340172838141441ULL, 144680345676217602ULL, 289360691352369924ULL, 578721382704674568ULL,
                                 1157442765409283856ULL, 2314885530818502432ULL, 4629771061636939584ULL, 9259542123273813888ULL,
                                 72340172854657281ULL, 144680345692602882ULL, 289360691368494084ULL, 578721382720276488ULL,
                                 1157442765423841296ULL, 2314885530830970912ULL, 4629771061645230144ULL, 9259542123273748608ULL,
                                 72340177082712321ULL, 144680349887234562ULL, 289360695496279044ULL, 578721386714368008ULL,
                                 1157442769150545936ULL, 2314885534022901792ULL, 4629771063767613504ULL, 9259542123257036928ULL,
                                 72341259464802561ULL, 144681423712944642ULL, 289361752209228804ULL, 578722409201797128ULL,
                                 1157443723186933776ULL, 2314886351157207072ULL, 4629771607097753664ULL, 9259542118978846848ULL,
                                 72618349279904001ULL, 144956323094725122ULL, 289632270724367364ULL, 578984165983651848ULL,
                                 1157687956502220816ULL, 2315095537539358752ULL, 4629910699613634624ULL, 9259541023762186368ULL,
                                 143553341945872641ULL, 215330564830528002ULL, 358885010599838724ULL, 645993902138460168ULL,
                                 1220211685215703056ULL, 2368647251370188832ULL, 4665518383679160384ULL, 9259260648297103488ULL,
                                 18302911464433844481ULL, 18231136449196065282ULL, 18087586418720506884ULL, 17800486357769390088ULL,
                                 17226286235867156496ULL, 16077885992062689312ULL, 13781085504453754944ULL, 9187484529235886208ULL};

const uint64_t king_att_table[64] = {770ULL, 1797ULL, 3594ULL, 7188ULL, 14376ULL, 28752ULL, 57504ULL, 49216ULL, 197123ULL, 460039ULL,
                                     920078ULL, 1840156ULL, 3680312ULL, 7360624ULL, 14721248ULL, 12599488ULL, 50463488ULL, 117769984ULL,
                                     235539968ULL, 471079936ULL, 942159872ULL, 1884319744ULL, 3768639488ULL, 3225468928ULL,
                                     12918652928ULL, 30149115904ULL, 60298231808ULL, 120596463616ULL, 241192927232ULL, 482385854464ULL,
                                     964771708928ULL, 825720045568ULL, 3307175149568ULL, 7718173671424ULL, 15436347342848ULL,
                                     30872694685696ULL, 61745389371392ULL, 123490778742784ULL, 246981557485568ULL, 211384331665408ULL,
                                     846636838289408ULL, 1975852459884544ULL, 3951704919769088ULL, 7903409839538176ULL,
                                     15806819679076352ULL, 31613639358152704ULL, 63227278716305408ULL, 54114388906344448ULL,
                                     216739030602088448ULL, 505818229730443264ULL, 1011636459460886528ULL, 2023272918921773056ULL,
                                     4046545837843546112ULL, 8093091675687092224ULL, 16186183351374184448ULL, 13853283560024178688ULL,
                                     144959613005987840ULL, 362258295026614272ULL, 724516590053228544ULL, 1449033180106457088ULL,
                                     2898066360212914176ULL, 5796132720425828352ULL, 11592265440851656704ULL, 4665729213955833856ULL};