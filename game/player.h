#ifndef __GAME_PLAYER_H__
#define __GAME_PLAYER_H__

#include "board.h"
#include "handtiles.h"
#include <string>

namespace mahjong {

class Player {
  public:
    Player(std::string _name) : _name(_name) {}
    std::string GetName() { return _name; }

    virtual int InterAction(const Board &b, const std::vector<std::vector<int>> &action) = 0;
    virtual int SelfAction(const Board &b, const std::vector<std::vector<int>> &action) = 0;

  protected:
    std::string _name;
};

} // namespace mahjong

#endif
