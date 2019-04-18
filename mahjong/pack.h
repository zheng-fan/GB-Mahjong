#ifndef __MAHJONG_PACK_H__
#define __MAHJONG_PACK_H__

#include "tile.h"
#include <vector>

namespace mahjong {

//牌组类型
#define PACK_TYPE_INVALID 0  // 无效牌组
#define PACK_TYPE_SHUNZI 1   // 顺子
#define PACK_TYPE_KEZI 2     // 刻子
#define PACK_TYPE_GANG 3     // 杠
#define PACK_TYPE_JIANG 4    // 将
#define PACK_TYPE_ZUHELONG 5 // 组合龙

//组合龙的六种组合形式
extern const long long ZuhelongBitmap[];

//牌组类：顺子、刻子、杠、将以及特殊形式（组合龙）
class Pack {
  public:
    Pack(int type = PACK_TYPE_INVALID, Tile tile = Tile(), int zuhelong_type = 0, int offer = 0) : _type(type), _tile(tile), _zuhelong_type(zuhelong_type), _offer(offer) {}

    int IsValid() const { return _type; }
    int GetType() const { return _type; }
    Tile GetMiddleTile() const { return _tile; }
    bool operator==(const Pack &p) const { return _type == p._type && _tile == p._tile; }

    std::vector<Tile> GetAllTile() const {
        std::vector<Tile> ret;
        switch (GetType()) {
        case PACK_TYPE_SHUNZI:
            ret.push_back(_tile.Pred());
            ret.push_back(_tile);
            ret.push_back(_tile.Succ());
            break;
        case PACK_TYPE_GANG:
            ret.push_back(_tile); //没有break
        case PACK_TYPE_KEZI:
            ret.push_back(_tile); //没有break
        case PACK_TYPE_JIANG:
            ret.push_back(_tile);
            ret.push_back(_tile);
            break;
        case PACK_TYPE_ZUHELONG:
            for (int i = TILE_1m; i <= TILE_9s; i++) {
                if (BITMAP(i) & GetZuhelongBitmap()) {
                    ret.push_back(Tile(i));
                }
            }
            break;
        default:
            break;
        }
        return ret;
    }
    int GetZuhelongType() const { return _zuhelong_type; }
    int GetZuhelongBitmap() const { return ZuhelongBitmap[_zuhelong_type]; }
    int GetOffer() const { return _offer; }

    int IsAnshou() const { return _offer == 0 || _offer == -1; }
    int HaveLastTile() const { return _offer < 0; }
    int IsShunzi() const { return _type == PACK_TYPE_SHUNZI; }
    int IsKezi() const { return _type == PACK_TYPE_KEZI; }
    int IsGang() const { return _type == PACK_TYPE_GANG; }
    int IsKeGang() const { return IsKezi() || IsGang(); }
    int IsJiang() const { return _type == PACK_TYPE_JIANG; }
    int IsZuhelong() const { return _type == PACK_TYPE_ZUHELONG; }

    void SetOffer(int offer) { _offer = offer; }
    void SetType(int type) { _type = type; }

  private:
    int _type;          //牌组类型
    Tile _tile;         //用一张牌表示牌组，如果是顺子，则表示中间的那张牌，若是组合龙则此项为空
    int _zuhelong_type; //组合龙类型
    int _offer;         //供牌信息，0为暗手，1、2、3为副露
                        //若为吃，则分别表示吃的牌是_tile的左、中、右
                        //若为碰或杠，则分别表示碰的牌来自于上家、对家、下家
                        //若为加杠，则用5、6、7来表示（便于mod 4）
                        //若包含最后所和的牌，使用-1表示自摸牌，-2表示铳和牌。实际上是Tile的_drawflag取负
};

} // namespace mahjong

#endif
