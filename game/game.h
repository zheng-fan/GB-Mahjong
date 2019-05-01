#ifndef __MAHJONG_GAME_H__
#define __MAHJONG_GAME_H__

#include <memory>
#include <player.h>

namespace mahjong {

class Game {
  public:
    Game(const Player &p0, const Player &p1, const Player &p2, const Player &p3) : _player({p0, p1, p2, p3}) {}
    virtual void Start() = 0;

  private:
    Player _player[4];
};

} // namespace mahjong

#endif
