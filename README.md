# GB-Mahjong

## Introduction

GB-Mahjong是一个国标麻将的C++库，实现了麻将中的一些基本类型的封装，提供了算番、计算听牌等多种实用工具。

## Mahjong Rules

国标麻将的规则主要基于国家体育总局于1998年颁布的《中国麻将竞赛规则（试行）》，但该规则存在一些定义不清、不严谨与不合理之处，且后续没有进行更新，因此国标麻将尚未有一个确定且权威的规则。尽管如此，随着多年来国标麻将的发展以及各大国际、国内赛事的举办，一些共识性意见逐渐形成。

GB-Mahjong的计番方式主要基于“世界麻将运动会”、“中国麻将牌王赛”等国标麻将大型赛事共识性意见，力求其准确性、正确性。主要参考资料为《国标麻将规则释义（网络对局版）》。

## Features

### 字符串与手牌对象的转换

GB-Mahjong支持以ASCII字符串形式构造手牌，其标准格式为：

```((\[([1-9]{3,4}[msp]|[ESWNCFP]{3,4})(,[123567])?\]|([ESWNCFPa-h]|[1-9]+[msp]))+(\|([ESWN]{2}[01]{4})(\|([a-h]{0,8}|[0-8]))?)?)```

举例如下：

* `123789s123789p33m`
* `11123456789999m|EE1000`
* `345s3p55p5567p[7777s]4p|EE0100|cbaghdfe`
* `[1111s,1][2222m,2][3333p,7][4444p]88s|EE0000|2`

规则解释如下：

* 基本格式为`立牌、副露|场况与和牌方式|花牌`，可以省略“花牌”或同时省略“场况与和牌方式”和“花牌”（默认“场况与和牌方式”为`EE0000`、无“花牌”）。
* 各麻将牌表示方法举例：`3m`（三万）、`6s`（六条）、`8p`（八饼）、`ESWNCFP`（东南西北中发白）、`abcdefgh`（梅兰竹菊春夏秋冬）。
* 相同花色序数牌可以连写：`2333m`（二万、三万、三万、三万）。
* 副露使用中括号`[]`表示，碰和杠的供牌者、吃哪张牌均使用`,序数`的形式表示：`[4444p]`（暗杠四饼）、`[1111s,1]`（明杠上家一条）、`[3333p,7]`（碰下家三饼后加杠）、`[345m,1]`（吃上家三万）、`[123p,3]`（吃上家三饼）。其中，序数只能为1、2、3、5、6、7，大于4的序数表示加杠。
* “立牌、副露”的最后一张如果为` `，则表示当前为13张手牌状态，否则最后一张表示摸到的牌或其他家打出的牌（会影响到算番）。
* “场况与和牌方式”`EE0000`分别表示：圈风、门风、是否为自摸、是否为绝张、是否为海底牌、是否为抢杠。
* “花牌”可以直接用数字表示拥有几张，采用“梅兰竹菊春夏秋冬”的顺序。

### 番数计算

GB-Mahjong支持番数计算，且对于除“整体属性类”以外的番种均可输出每个番种是由哪些麻将牌所组成。

例如对于`[123m,1][123p,1]123m12p44s3p`（(🀇)🀈🀉, (🀙)🀚🀛 | 🀇🀈🀉🀓🀓🀙🀚🀛），算番器计算得知这副牌共有18番，具体番种及其构成为：

```
         小于五 12番
           平和  2番
         一般高  1番 (🀇)🀈🀉, 🀇🀈🀉
         一般高  1番 (🀙)🀚🀛, 🀙🀚🀛
         喜相逢  1番 (🀇)🀈🀉, (🀙)🀚🀛
           边张  1番 🀙🀚🀛
```

### 听牌计算

GB-Mahjong支持听牌计算，手牌中已有4张的牌不会被计入。

例如对于`3344455566667m `（🀉🀉🀊🀊🀊🀋🀋🀋🀌🀌🀌🀌🀍），程序可输出这副牌共听🀈🀉🀊🀋🀍🀎等6张牌。

## Usage

```cpp
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
```

## Dependencies

G++ >= 4.9

## Build & Check

Just `make` it:

```
make -j
```

测试：

```
make check
```

GB-Mahjong内置100余个test case以保证正确性，部分test case列举如下：

```
Test case 145: [CCCC][FFFF][PPPP][EEEE]NN|EE1011|8, ret = 0
 - ✅  Passed: [string check], Handtiles: 🀫🀄🀄🀫, 🀫🀅🀅🀫, 🀫🀆🀆🀫, 🀫🀀🀀🀫 | 🀃🀃 | 🀢🀣🀤🀥🀦🀧🀨🀩
 - ✅  standard string = [CCCC][FFFF][PPPP][EEEE]NN|EE1011|abcdefgh
 - ✅  Passed: [fan check], count of fan is 332 and fan is:
         大三元 88番 🀫🀄🀄🀫, 🀫🀅🀅🀫, 🀫🀆🀆🀫
           四杠 88番 🀫🀄🀄🀫, 🀫🀅🀅🀫, 🀫🀆🀆🀫, 🀫🀀🀀🀫
         字一色 64番
         四暗刻 64番 🀫🀄🀄🀫, 🀫🀅🀅🀫, 🀫🀆🀆🀫, 🀫🀀🀀🀫
       妙手回春  8番
       杠上开花  8番
         圈风刻  2番 🀫🀀🀀🀫
         门风刻  2番 🀫🀀🀀🀫
           花牌  1番 × 8
Test case 178: [1111s][2222m,1][4444p,3]333p88s, ret = 0
 - ✅  Passed: [string check], Handtiles: 🀫🀐🀐🀫, (🀈)🀈🀈🀈, 🀜🀜🀜(🀜) | 🀗🀛🀛🀛🀗 | 
 - ✅  standard string = [1111s][2222m,1][4444p,3]8s333p8s|EE0000|
 - ✅  Passed: [fan check], count of fan is 51 and fan is:
           三杠 32番 🀫🀐🀐🀫, (🀈)🀈🀈🀈, 🀜🀜🀜(🀜)
     三色三节高  8番 🀫🀐🀐🀫, (🀈)🀈🀈🀈, 🀛🀛🀛
         碰碰和  6番
         双暗刻  2番 🀫🀐🀐🀫, 🀛🀛🀛
         幺九刻  1番 🀫🀐🀐🀫
           无字  1番
         单钓将  1番 🀗🀗
```

## License

[MIT License](LICENSE)
