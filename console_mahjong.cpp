#include "console_game.h"

int main() {
    mahjong::ConsoleGame game(mahjong::Player("fz"), mahjong::Player("AI1"), mahjong::Player("AI2"), mahjong::Player("AI3"));
    game.Start();
    return 0;
}
