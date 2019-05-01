#ifndef __MAHJONG_CONSOLE_GAME_H__
#define __MAHJONG_CONSOLE_GAME_H__

#include "console.h"
#include "debug.h"
#include "game.h"
#include <iostream>

namespace mahjong {

class ConsoleGame : public Game {
  public:
    ConsoleGame(const Player &p0, const Player &p1, const Player &p2, const Player &p3) : Game(p0, p1, p2, p3) {}
    virtual void Start() {
        PrintMahjong(_con);
        std::cout << _con.GetKeyboardInput();
    }

  private:
    Console _con;
};

} // namespace mahjong

#endif
