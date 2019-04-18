#include "debug.h"
#include "tile.h"
#include <ncursesw/ncurses.h>

#include <bits/stdc++.h>
using namespace std;

void PrintMahjong(Console &con) {
    for (int i = 1; i < TILE_SIZE; i++)
        con.PrintTile(Tile(i));
    addch('\n');
    addch('\n');
    attron(A_BLINK);
    printw("汉字");
    printw("测试");
    attroff(A_BLINK);
    printw("汉字");
    // for (int i = 0; i < TILE_SIZE; i++)
    //     Console::PrintTile(i, A_UNDERLINE | A_ITALIC | A_BOLD);
    // addch('\n');
    // addch('\n');
}

void StdPrintTile(const Tile &t) {
    printf("%s ", t.UTF8());
}

void StdPrintHandtiles(Handtiles &h) {
    int flag = 0;
    for (auto p : h.fulu) {
        printf("%s%s", flag ? ", " : "", PackToEmojiString(p).c_str());
        flag = 1;
    }
    printf(" | ");
    for (size_t i = 0; i < h.lipai.size(); i++) {
        const Tile &t = h.lipai[i];
        // if (i == h.lipai.size() - 1) {
        //     putchar(' ');
        // }
        StdPrintTile(t);
    }
    printf(" | ");
    for (auto t : h.huapai) {
        StdPrintTile(t);
    }
    puts("");
}
