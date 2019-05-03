#include "console_game.h"
#include "console_player.h"
#include "random_action_player.h"

namespace mahjong {

Player *ConsoleGame::_ParsePairString(const std::pair<std::string, std::string> &pr) {
    if (pr.first == "ConsolePlayer") {
        return new ConsolePlayer(pr.second, *this);
    } else if (pr.first == "RandomActionPlayer") {
        return new RandomActionPlayer(pr.second);
    }
    //  else if (pr.first == "SimpleAIPlayer") {
    //     return new SimpleAIPlayer(pr.second);
    // } else if (pr.first == "AIPlayer") {
    //     return new AIPlayer(pr.second);
    // }
}

} // namespace mahjong
