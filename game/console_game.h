#ifndef __GAME_CONSOLE_GAME_H__
#define __GAME_CONSOLE_GAME_H__

#include "console.h"
#include "debug.h"
#include "game.h"
#include <iostream>
#include <string>

namespace mahjong {

class ConsoleGame : public Game {
  public:
    ConsoleGame(const std::vector<std::pair<std::string, std::string>> &player_info) {
        for (size_t i = 0; i < player_info.size(); i++) {
            _player[i] = std::shared_ptr<Player>(_ParsePairString(player_info[i]));
        }
    }
    virtual void Start() {
        PrintMahjong(_con);
        std::cout << _con.GetKeyboardInput();
    }

  private:
    Console _con;
    Player *_ParsePairString(const std::pair<std::string, std::string> &pr);
};

} // namespace mahjong

#endif
