#ifndef __GAME_GAME_H__
#define __GAME_GAME_H__

#include "board.h"
#include "player.h"
#include <memory>

namespace mahjong {

class Game {
  public:
    virtual void Start() = 0;

  protected:
    std::shared_ptr<Player> _player[4];
    Board _board;
};

} // namespace mahjong

#endif
