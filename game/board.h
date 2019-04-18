#ifndef __MAHJONG_BOARD_H__
#define __MAHJONG_BOARD_H__

#include "fan.h"
#include "tile.h"

namespace mahjong {

//牌局类
class Board {
  public:
    Board() {}

  private:
    Fan _fan; //算番器

    enum BOARD_STATUS {} _status; //牌局状态机
};

} // namespace mahjong

#endif
