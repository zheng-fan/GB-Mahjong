#include "console.h"

namespace mahjong {

const std::string PackToEmojiString(const Pack &p) {
    std::string ret;
    Tile t = p.GetMiddleTile();
    int offer = p.GetOffer();
    if (offer < 0) { //和的最后一张视为暗手来输出（包括铳和）
        offer = 0;
    }
    switch (p.GetType()) {
    case PACK_TYPE_SHUNZI:
        if (offer) {
            ret = ret + "(" + TileToEmojiString(t.GetTileUsingOffset(offer - 2)) + ")";
        }
        for (int i = -1; i <= 1; i++) {
            if (i != offer - 2) {
                ret += TileToEmojiString(t.GetTileUsingOffset(i));
            }
        }
        break;
    case PACK_TYPE_KEZI:
        for (int i = -1; i <= 1; i++) {
            if (i != offer - 2) {
                ret += TileToEmojiString(t);
            } else {
                ret = ret + "(" + TileToEmojiString(t) + ")";
            }
        }
        break;
    case PACK_TYPE_GANG:
        if (offer == 0) {
            ret = ret + TileToEmojiString(Tile(TILE_MAJIANG)) + TileToEmojiString(t) + TileToEmojiString(t) + TileToEmojiString(Tile(TILE_MAJIANG));
        } else {
            int flag[4] = {};
            for (int i = 0; i < 4; i++) {
                if (i == offer - 1) {
                    flag[i] = 1;
                }
            }
            std::swap(flag[2], flag[3]);
            offer -= 4;
            for (int i = 0; i < 4; i++) {
                if (i == offer - 1 || i == offer) {
                    flag[i] = 1;
                }
            }
            for (int i = 0; i < 4; i++) {
                if (flag[i] == 0) {
                    ret += TileToEmojiString(t);
                } else {
                    ret = ret + "(" + TileToEmojiString(t) + ")";
                }
            }
        }
        break;
    case PACK_TYPE_JIANG:
        ret = ret + TileToEmojiString(t);
        ret = ret + TileToEmojiString(t);
        break;
    case PACK_TYPE_ZUHELONG:
        for (int i = TILE_1m; i <= TILE_9p; i++) {
            if (BITMAP(i) & p.GetZuhelongBitmap()) {
                ret += TileToEmojiString(Tile(i));
            }
        }
        break;
    default:
        break;
    }
    return ret;
}
const std::string TileToEmojiString(const Tile &t) {
    return std::string() + t.UTF8() + ' ';
}

} // namespace mahjong
