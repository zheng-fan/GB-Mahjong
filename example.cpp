#include "console.h"
#include "fan.h"
#include "handtiles.h"
#include "print.h"
#include <iostream>
#include <vector>

int main() {
    // 构造手牌
    mahjong::Handtiles ht;
    ht.StringToHandtiles("[456s,1][456s,1][456s,3]45s55m |EE0000|fah");

    // 计算听牌
    mahjong::Fan fan;
    std::vector<mahjong::Tile> ting = fan.CalcTing(ht);
    for (auto t : ting) {
        mahjong::StdPrintTile(t);
    }
    std::cout << std::endl;

    // 判断是否铳和
    ht.SetTile(mahjong::Tile(TILE_8s));
    printf("%s 是否和牌：%d\n", ht.HandtilesToString().c_str(), fan.JudgeHu(ht));

    // 摸牌并算番
    ht.DiscardTile(); // 出牌
    ht.DrawTile(mahjong::Tile(TILE_6s)); // 摸牌
    printf("手牌：%s\n", ht.HandtilesToString().c_str()); // 输出手牌
    fan.CountFan(ht); // 计番
    printf("总番数：%d\n", fan.tot_fan_res); // 总番数
    for (int i = 1; i < mahjong::FAN_SIZE; i++) { // 输出所有的番
        for (size_t j = 0; j < fan.fan_table_res[i].size(); j++) {
            printf("%s %d番", mahjong::FAN_NAME[i], mahjong::FAN_SCORE[i]);
            std::string pack_string;
            for (auto pid : fan.fan_table_res[i][j]) { // 获取该番种具体的组合方式
                pack_string += " " + mahjong::PackToEmojiString(fan.fan_packs_res[pid]);
            }
            printf("%s\n", pack_string.c_str());
        }
    }

    return 0;
}
