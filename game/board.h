#ifndef __GAME_BOARD_H__
#define __GAME_BOARD_H__

#include "fan.h"
#include "tile.h"

namespace mahjong {

#define ACTION_INVALID 0
#define ACTION_CHI 1
#define ACTION_PENG 2
#define ACTION_MINGGANG 3
#define ACTION_JIAGANG 4
#define ACTION_ANGANG 5
#define ACTION_BUHUA 6
#define ACTION_HU 7
#define ACTION_DISCARDTILE 8

enum board_status_t {
    BOARD_STATUS_WAITING_FOR_START,
}; //牌局状态机

//牌局类
class Board {
  public:
    Board() {}
    void Next() {
        switch (_status) {
        case BOARD_STATUS_WAITING_FOR_START:
            break;
        default:
            break;
        }
    }

  private:
    Fan _fan; //算番器
    board_status_t _status;
};

} // namespace mahjong

#endif
