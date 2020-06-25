#ifndef __CONSOLE_CONSOLE_H__
#define __CONSOLE_CONSOLE_H__

#include "pack.h"
#include "tile.h"
#include <cstdio>
#include <string>

namespace mahjong {

const std::string PackToEmojiString(const Pack &p);
const std::string TileToEmojiString(const Tile &p);

} // namespace mahjong

#endif
