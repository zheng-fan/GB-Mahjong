#ifndef __MAHJONG_PLAYER_H__
#define __MAHJONG_PLAYER_H__

#include "handtiles.h"
#include <string>

namespace mahjong {

class Player {
  public:
    Player(std::string _name = "Unknown Player") : _name(_name) {}
    std::string GetName() { return _name; }

  private:
    std::string _name;
};

} // namespace mahjong

#endif
