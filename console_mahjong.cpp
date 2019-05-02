#include "console_game.h"

int main() {
    mahjong::ConsoleGame game({{"ConsolePlayer", "You"}, {"RandomActionPlayer", "AI 1"}, {"RandomActionPlayer", "AI 2"}, {"RandomActionPlayer", "AI 3"}});
    game.Start();
    return 0;
}
