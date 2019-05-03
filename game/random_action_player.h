#ifndef __GAME_RANDOM_ACTION_PLAYER_H__
#define __GAME_RANDOM_ACTION_PLAYER_H__

#include "player.h"

namespace mahjong {

class RandomActionPlayer : public Player {
  public:
    RandomActionPlayer(std::string _name) : Player(_name) {
        srand(time(NULL));
    }
    std::string GetName() { return _name; }

    virtual int InterAction(const Board &b, const std::vector<std::vector<int>> &action) {
        for (size_t i = 0; i < action.size(); i++) {
            if (action[i][0] == ACTION_HU) {
                return i;
            }
        }
        return rand() % action.size();
    }
    virtual int SelfAction(const Board &b, const std::vector<std::vector<int>> &action) {
        for (size_t i = 0; i < action.size(); i++) {
            if (action[i][0] == ACTION_HU) {
                return i;
            }
        }
        return rand() % action.size();
    }

  private:
};

} // namespace mahjong

#endif
