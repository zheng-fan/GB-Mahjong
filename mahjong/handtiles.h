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
    std::vector<Pack> fulu;                    //副露和暗杠
    std::vector<Tile> lipai;                   //立牌（最后一张是TILE_INVALID或自摸牌/他人打出的牌，保证副露+立牌为14张）
    std::vector<Tile> huapai;                  //花牌
    std::unordered_map<int, int> fulu_table;   //副露中的所有牌对应的数量
    std::unordered_map<int, int> lipai_table;  //立牌中的所有牌对应的数量
    std::unordered_map<int, int> huapai_table; //花牌中的所有牌对应的数量

    long long FuluBitmap() const {
        long long bitmap = 0;
        for (auto p : fulu) {
            Tile mt = p.GetMiddleTile();
            switch (p.GetType()) {
            case PACK_TYPE_SHUNZI:
                bitmap |= mt.GetBitmap() | mt.Pred().GetBitmap() | mt.Succ().GetBitmap();
                break;
            case PACK_TYPE_KEZI:
            case PACK_TYPE_GANG:
            case PACK_TYPE_JIANG:
                bitmap |= mt.GetBitmap();
                break;
            case PACK_TYPE_ZUHELONG: //副露不可能是组合龙
            default:
                break;
            }
        }
        return bitmap;
    }
    long long LipaiBitmap() const {
        long long bitmap = 0;
        for (auto t : lipai) {
            bitmap |= t.GetBitmap();
        }
        return bitmap;
    }

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
    int LipaiTileCount(const Tile &tile) const { //立牌中某个牌的数量
        auto ret = lipai_table.find(tile.GetId());
        return ret != lipai_table.end() ? ret->second : 0;
    }
    int FuluTileCount(const Tile &tile) const { //副露中某个牌的数量
        auto ret = fulu_table.find(tile.GetId());
        return ret != fulu_table.end() ? ret->second : 0;
    }
    int HandTileCount(const Tile &tile) const { //手牌中某个牌的数量
        return LipaiTileCount(tile) + FuluTileCount(tile);
    }
    int HuapaiCount() const {
        int cnt = 0;
        for (int i = TILE_MEI; i <= TILE_DONG; i++) {
            cnt += huapai_table.find(i)->second;
        }
        return cnt;
    }

    std::string HandtilesToString() const { //由手牌构造字符串
        std::string ret;
        //输出所有副露
        for (auto p : fulu) {
            Tile MiddleTile = p.GetMiddleTile();
            std::vector<Tile> v = p.GetAllTile();
            ret += '[';
            for (auto t : v) {
                ret += t.TileChar();
            }
            if (MiddleTile.IsShu()) {
                ret += MiddleTile.SuitChar();
            }
            if (p.GetOffer()) {
                ret += ',';
                ret += std::to_string(p.GetOffer());
            }
            ret += ']';
        }
        //输出所有立牌，花色相同的序数牌花色只输出一次
        int flag_first_numbered_tile = 1;
        for (size_t i = 0; i < lipai.size(); i++) {
            if (!flag_first_numbered_tile) { //凡是刚输出的是数牌的情况下
                //如果当前是第14张牌，或者它不是数牌，或者它与前面一张牌的花色不同
                //它前面的数牌都需要输出花色
                //（第14张牌不可与前面的数牌合在一起只输出一次花色）
                if (i + 1 + fulu.size() * 3 == 14 || !lipai[i].IsShu() || lipai[i].Suit() != lipai[i - 1].Suit()) {
                    ret += lipai[i - 1].SuitChar();
                }
            }
            flag_first_numbered_tile = !(lipai[i].IsShu());
            ret += lipai[i].TileChar();
        }
        if (!flag_first_numbered_tile) { //最后一张如果是序数牌则需要补一个它的花色
            ret += GetLastLipai().SuitChar();
        }
        //输出和牌时的情况：圈风、门风、自摸、绝张、海底、杠牌
        ret += '|';
        ret += Tile(GetQuanfeng()).TileChar();
        ret += Tile(GetMenfeng()).TileChar();
        ret += std::to_string(IsZimo());
        ret += std::to_string(IsJuezhang());
        ret += std::to_string(IsHaidi());
        ret += std::to_string(IsGang());
        //输出花牌
        ret += '|';
        for (auto t : huapai) {
            ret += t.TileChar();
        }
        return ret;
    }
    int StringToHandtiles(const std::string &s_ori) { //由字符串构造手牌
        std::string s;
        for (size_t i = 0; i < s_ori.length(); i++) {
            if (s_ori[i] != ' ') {
                s += s_ori[i];
            }
        }
        //初步检测字符串是否合法
        std::regex pattern(R"((\[([1-9]{3,4}[msp]|[ESWNCFP]{3,4})(,[123567])?\]|([ESWNCFPa-h]|[1-9]+[msp]))+(\|([ESWN]{2}[01]{4})(\|([a-h]{0,8}|[0-8]))?)?)");
        if (!std::regex_match(s, pattern)) {
            return -1; //【错误】：字符串非法
        }
        ClearAndSetDefault(); //清空
        //牌从字符到编码的映射
        std::unordered_map<char, int> mp{{TILE_CHAR_WAN, TILE_1m}, {TILE_CHAR_TIAO, TILE_1s}, {TILE_CHAR_BING, TILE_1p}, {TILE_CHAR_E, TILE_E}, {TILE_CHAR_S, TILE_S}, {TILE_CHAR_W, TILE_W}, {TILE_CHAR_N, TILE_N}, {TILE_CHAR_C, TILE_C}, {TILE_CHAR_F, TILE_F}, {TILE_CHAR_P, TILE_P}, {TILE_CHAR_MEI, TILE_MEI}};
        int part = 0;         //正在处理的是副露立牌(0)、和牌情况(1)还是花牌(2)
        int is_fulu = 0;      //当前处理的是否为一组副露
        int handle_offer = 0; //当前是否正在处理副露的供牌信息
        int offer = 0;        //当前处理的副露的供牌信息
        std::string nums;     //暂存连续的序数牌的序数
        std::string chars;    //暂存字牌
        char char_suit = 0;
        for (auto c : s) {
            switch (c) {
            case '[':
                is_fulu = 1;
                break;
            case ']': {
                int is_chars; //0表示nums，1表示chars
                int tile_code;
                if (nums.length()) {
                    is_chars = 0;
                    tile_code = mp[char_suit] - 1 + nums[1] - '0';
                } else {
                    is_chars = 1;
                    tile_code = mp[chars[1]];
                }
                std::string &tiles = is_chars ? chars : nums;
                Pack p(PACK_TYPE_INVALID, Tile(tile_code));
                if (tiles.length() == 3) {   //处理三张的副露
                    if (handle_offer == 0) { //如果没有供牌信息，则默认为1
                        offer = 1;
                    }
                    if (offer > 3) {
                        return -2; //【错误】：三张一组的副露不能是加杠
                    }
                    if (!is_chars && tiles[1] == tiles[0] + 1 && tiles[1] == tiles[2] - 1) { //吃
                        p.SetType(PACK_TYPE_SHUNZI);
                    } else if (tiles[1] == tiles[0] && tiles[1] == tiles[2]) { //碰
                        p.SetType(PACK_TYPE_KEZI);
                    } else {
                        return -3; //【错误】：存在非法的三张一组的副露
                    }
                } else if (tiles.length() == 4) { //处理四张的副露
                    if (handle_offer == 0) {      //如果没有供牌信息，则默认为0（暗杠）
                        offer = 0;
                    }
                    if (tiles[1] == tiles[0] && tiles[1] == tiles[2] && tiles[1] == tiles[3]) { //杠
                        p.SetType(PACK_TYPE_GANG);
                    } else {
                        return -4; //【错误】：存在非法的四张一组的副露
                    }
                }
                p.SetOffer(offer);
                fulu.push_back(p);

                //恢复状态
                is_fulu = 0;
                handle_offer = 0;
                tiles.clear();
                char_suit = 0;
                break;
            }
            case ',':
                handle_offer = 1;
                break;
            case '0' ... '9':
                if (part == 0) {
                    if (is_fulu) {
                        if (!handle_offer) {
                            nums += c;
                        } else {
                            offer = c - '0';
                        }
                    } else {
                        nums += c;
                    }
                } else if (part == 1) {
                    nums += c;
                } else if (part == 2) {
                    for (int i = 0; i < c - '0'; i++) {
                        huapai.push_back(Tile(mp[TILE_CHAR_MEI] + i));
                    }
                }
                break;
            case TILE_CHAR_E:
            case TILE_CHAR_S:
            case TILE_CHAR_W:
            case TILE_CHAR_N:
            case TILE_CHAR_C:
            case TILE_CHAR_F:
            case TILE_CHAR_P:
                if (part == 0) {
                    if (is_fulu) {
                        chars += c;
                        char_suit = 'z';
                    } else {
                        lipai.push_back(Tile(mp[c]));
                    }
                } else if (part == 1) {
                    chars += c;
                }
                break;
            case TILE_CHAR_WAN:
            case TILE_CHAR_TIAO:
            case TILE_CHAR_BING:
                if (is_fulu) {
                    char_suit = c;
                } else {
                    for (size_t i = 0; i < nums.length(); i++) {
                        lipai.push_back(Tile(mp[c] - 1 + nums[i] - '0'));
                    }
                    nums.clear();
                }
                break;
            case '|':
                part++;
                break;
            case TILE_CHAR_MEI ... TILE_CHAR_DONG:
                if (part == 0) {
                    lipai.push_back(Tile(mp[TILE_CHAR_MEI] + c - TILE_CHAR_MEI));
                } else if (part == 2) {
                    huapai.push_back(Tile(mp[TILE_CHAR_MEI] + c - TILE_CHAR_MEI));
                }
                break;
            default:
                return -999; //【错误】：未知。正常不会运行到这里
                break;
            }
        }
        //设置牌局状态：圈风门风和和牌状态
        if (part >= 1) {
            SetQuanfeng(mp[chars[0]]);
            SetMenfeng(mp[chars[1]]);
            SetZimo(nums[0] - '0');
            SetJuezhang(nums[1] - '0');
            SetHaidi(nums[2] - '0');
            SetGang(nums[3] - '0');
        }
        //设置最后一张牌的摸牌状态，用于算番
        if (IsZimo()) {
            LastLipai().SetZimo();
        } else {
            LastLipai().SetChonghu();
        }
        //检查手牌张数是否为14或13（副露都算3张）
        if (fulu.size() * 3 + lipai.size() == 13) {
            lipai.push_back(Tile(TILE_INVALID)); //如果是13张则加一张占位牌
        } else if (fulu.size() * 3 + lipai.size() != 14) {
            return -5; //【错误】：手牌张数不正确
        }
        if (GenerateTable()) {
            return -6; //【错误】：手牌中存在数量非法的麻将牌
        }
        //检查和牌状态是否合法
        if (IsGang()) {     //杠状态下，
            if (IsZimo()) { //若为自摸，则为杠上开花：手里必须有杠（更严格的话是最后一个副露必须为杠）
                if (!any_of(fulu.begin(), fulu.end(), [](const Pack &p) -> int { return p.IsGang(); })) {
                    return -7; //【错误】：和牌状态非法
                }
            } else { //若为铳和，则为抢杠和：非海底状态（能加杠说明不是海底），所和的牌必须手里没有
                if (IsHaidi() || HandTileCount(GetLastLipai()) > 1) {
                    return -7; //【错误】：和牌状态非法
                }
            }
        }
        if (IsJuezhang()) { //绝张状态下，立牌必须没有这张牌
            if (LipaiTileCount(GetLastLipai()) > 1) {
                return -7; //【错误】：和牌状态非法
            }
        }
        //对立牌进行排序
        SortLipaiWithoutLastOne();
        return 0;
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
