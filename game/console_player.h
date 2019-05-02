#ifndef __GAME_CONSOLE_PLAYER_H__
#define __GAME_CONSOLE_PLAYER_H__

#include "console_game.h"
#include "player.h"

namespace mahjong {

class ConsolePlayer : public Player {
  public:
    ConsolePlayer(std::string _name, ConsoleGame &_game) : Player(_name), _game(_game) {}
    std::string GetName() { return _name; }

    virtual int InterAction(const Board &b, const std::vector<std::vector<int>> &action) {
    }
    virtual int SelfAction(const Board &b, const std::vector<std::vector<int>> &action) {
    }

  private:
    ConsoleGame &_game;
};

} // namespace mahjong

#endif
