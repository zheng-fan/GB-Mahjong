#include "pack.h"

namespace mahjong {

//组合龙的六种组合形式
const long long ZuhelongBitmap[] = {
    0, //用来标识没有组合龙
    1ll << TILE_1m | 1ll << TILE_4m | 1ll << TILE_7m |
        1ll << TILE_2s | 1ll << TILE_5s | 1ll << TILE_8s |
        1ll << TILE_3p | 1ll << TILE_6p | 1ll << TILE_9p, // 147m 258s 369p
    1ll << TILE_1m | 1ll << TILE_4m | 1ll << TILE_7m |
        1ll << TILE_3s | 1ll << TILE_6s | 1ll << TILE_9s |
        1ll << TILE_2p | 1ll << TILE_5p | 1ll << TILE_8p, // 147m 369s 258p
    1ll << TILE_2m | 1ll << TILE_5m | 1ll << TILE_8m |
        1ll << TILE_1s | 1ll << TILE_4s | 1ll << TILE_7s |
        1ll << TILE_3p | 1ll << TILE_6p | 1ll << TILE_9p, // 258m 147s 369p
    1ll << TILE_2m | 1ll << TILE_5m | 1ll << TILE_8m |
        1ll << TILE_3s | 1ll << TILE_6s | 1ll << TILE_9s |
        1ll << TILE_1p | 1ll << TILE_4p | 1ll << TILE_7p, // 258m 369s 147p
    1ll << TILE_3m | 1ll << TILE_6m | 1ll << TILE_9m |
        1ll << TILE_1s | 1ll << TILE_4s | 1ll << TILE_7s |
        1ll << TILE_2p | 1ll << TILE_5p | 1ll << TILE_8p, // 369m 147s 258p
    1ll << TILE_3m | 1ll << TILE_6m | 1ll << TILE_9m |
        1ll << TILE_2s | 1ll << TILE_5s | 1ll << TILE_8s |
        1ll << TILE_1p | 1ll << TILE_4p | 1ll << TILE_7p, // 369m 258s 147p
};

} // namespace mahjong
