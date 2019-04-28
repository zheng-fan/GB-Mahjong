#ifndef __MAHJONG_PLAYER_H__
#define __MAHJONG_PLAYER_H__

#include "pack.h"
#include <algorithm>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

namespace mahjong {

//门风圈风
#define WIND_E TILE_E
#define WIND_S TILE_S
#define WIND_W TILE_W
#define WIND_N TILE_N

//手牌类
class Handtiles {
  public:
    std::vector<Pack> fulu;                    //副露（包括暗杠）
    std::vector<Tile> lipai;                   //立牌（最后一张是TILE_INVALID或自摸牌/他人打出的牌，保证副露+立牌为14张）
    std::vector<Tile> huapai;                  //花牌
    std::unordered_map<int, int> fulu_table;   //副露中的所有牌对应的数量
    std::unordered_map<int, int> lipai_table;  //立牌中的所有牌对应的数量
    std::unordered_map<int, int> huapai_table; //花牌中的所有牌对应的数量

    //返回副露牌的Bitmap
    long long FuluBitmap() const;
    //返回立牌的Bitmap
    long long LipaiBitmap() const;
    //返回立牌中某个牌的数量
    int LipaiTileCount(const Tile &tile) const;
    //返回副露中某个牌的数量
    int FuluTileCount(const Tile &tile) const;
    //返回手牌中某个牌的数量
    int HandTileCount(const Tile &tile) const;
    //返回花牌的总数量
    int HuapaiCount() const;

    //由手牌构造字符串
    std::string HandtilesToString() const;
    //由字符串构造手牌
    int StringToHandtiles(const std::string &s_ori);

    int GenerateTable() { //生成副露、立牌和花牌的表，返回-1则表示张数有误
        for (auto p : fulu) {
            std::vector<Tile> v = p.GetAllTile();
            for (auto t : v) {
                fulu_table[t.GetId()]++;
            }
        }
        for (auto t : lipai) {
            lipai_table[t.GetId()]++;
        }
        for (auto t : huapai) {
            huapai_table[t.GetId()]++;
        }
        for (int i = TILE_1m; i < TILE_SIZE; i++) {
            if (fulu_table[i] + lipai_table[i] > 4) {
                return -1;
            }
        }
        for (int i = TILE_MEI; i <= TILE_DONG; i++) {
            if (lipai_table[i] + huapai_table[i] > 1) {
                return -1;
            }
        }
        return 0;
    }
    void DrawTile(const Tile &tile) { //抓牌（自摸牌）
        SetLastLipai(tile);
        LastLipai().SetZimo();
        lipai_table[tile.GetId()]++;
    }
    void SetTile(const Tile &tile) { //抓牌（铳和牌）
        SetLastLipai(tile);
        LastLipai().SetChonghu();
        lipai_table[tile.GetId()]++;
    }
    Tile DiscardTile() { //出牌
        Tile tile(GetLastLipai().GetId());
        SetLastLipai(Tile(TILE_INVALID));
        lipai_table[tile.GetId()]--;
        return tile;
    }

    void SortLipaiWithoutLastOne() {
        std::sort(lipai.begin(), --lipai.end());
    }
    void SortLipaiAll() {
        std::sort(lipai.begin(), lipai.end());
    }
    void ClearAndSetDefault() {
        //清空手牌
        fulu.clear();
        lipai.clear();
        huapai.clear();
        fulu_table.clear();
        lipai_table.clear();
        huapai_table.clear();
        //设置默认状态
        SetQuanfeng(WIND_E);
        SetMenfeng(WIND_E);
        SetZimo(0);
        SetJuezhang(0);
        SetHaidi(0);
        SetGang(0);
    }

    int GetQuanfeng() const { return _quanfeng; }
    int GetMenfeng() const { return _menfeng; }
    int IsZimo() const { return _zimo; }
    int IsJuezhang() const { return _juezhang; }
    int IsHaidi() const { return _haidi; }
    int IsGang() const { return _gang; }
    int IsMenqing() const {
        return std::all_of(fulu.begin(), fulu.end(), [](const Pack &p) -> int { return p.IsAnshou(); });
    }
    int IsTotallyFulu() const {
        return fulu.size() == 4 && std::all_of(fulu.begin(), fulu.end(), [](const Pack &p) -> int { return !p.IsAnshou(); });
    }
    int NoFulu() const { return fulu.size() == 0; } //也没有暗杠

    void SetQuanfeng(int val) { _quanfeng = val; }
    void SetMenfeng(int val) { _menfeng = val; }
    void SetZimo(int val) { _zimo = val; }
    void SetJuezhang(int val) { _juezhang = val; }
    void SetHaidi(int val) { _haidi = val; }
    void SetGang(int val) { _gang = val; }

    void SetLastLipai(const Tile &t) { lipai[lipai.size() - 1] = t; }
    Tile &LastLipai() { return lipai[lipai.size() - 1]; }
    const Tile &GetLastLipai() const { return lipai[lipai.size() - 1]; }

  private:
    int _quanfeng; //圈风
    int _menfeng;  //门风
    int _zimo;     //1表示自摸，0表示铳和
    int _juezhang; //是否为绝张
    int _haidi;    //是否为海底牌状态（海底捞月、妙手回春）
    int _gang;     //是否为杠牌状态（抢杠和、杠上开花）
};

} // namespace mahjong

#endif
