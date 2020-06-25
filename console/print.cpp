#include "print.h"
#include "console.h"
#include "tile.h"

namespace mahjong {

void StdPrintTile(const Tile &t) {
    printf("%s ", t.UTF8());
}

void StdPrintHandtiles(Handtiles &h) {
    int flag = 0;
    for (auto p : h.fulu) {
        printf("%s%s", flag ? ", " : "", PackToEmojiString(p).c_str());
        flag = 1;
    }
    printf(" | ");
    for (size_t i = 0; i < h.lipai.size(); i++) {
        const Tile &t = h.lipai[i];
        StdPrintTile(t);
    }
    printf(" | ");
    for (auto t : h.huapai) {
        StdPrintTile(t);
    }
    puts("");
}

} // namespace mahjong
