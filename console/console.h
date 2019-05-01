#ifndef __CONSOLE_CONSOLE_H__
#define __CONSOLE_CONSOLE_H__

#include "pack.h"
#include "tile.h"
#include <cstdio>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <string>
#include <unistd.h>

namespace mahjong {

const std::string PackToEmojiString(const Pack &p);
const std::string TileToEmojiString(const Tile &p);

class Console {
  public:
    Console() {
        setlocale(LC_ALL, ""); //必须在初始化之前完成
        initscr();
        noecho();
        cbreak(); /* Line buffering disabled. pass on everything */
    }
    ~Console() {
        endwin();
    }
    void PrintTile(const Tile &tile, unsigned attr = 0) {
        printw("%s", tile.UTF8());
        addch(' ');
    }
    int GetKeyboardInput() {
        return getch();
    }

  private:
};

} // namespace mahjong

#endif
