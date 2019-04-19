#ifndef __TOOLS_DEBUG_H__
#define __TOOLS_DEBUG_H__

#include "console.h"
#include "handtiles.h"
#include <chrono>

using namespace console;

#define debug puts("-----")

void PrintMahjong(Console &con);
void StdPrintTile(const Tile &t);
void StdPrintHandtiles(Handtiles &h);
void UnitTest();

// #define DEBUG_DFS
// #define DEBUG_ASSCOM_TABLE
// #define DEBUG_DFS_CNT

#endif
