#ifndef __MAHJONG_TILE_H__
#define __MAHJONG_TILE_H__

namespace mahjong {

// 麻将牌
#define TILE_INVALID 0  //无效牌
#define TILE_1m 1       //万
#define TILE_2m 2       //
#define TILE_3m 3       //
#define TILE_4m 4       //
#define TILE_5m 5       //
#define TILE_6m 6       //
#define TILE_7m 7       //
#define TILE_8m 8       //
#define TILE_9m 9       //
#define TILE_1s 10      //条
#define TILE_2s 11      //
#define TILE_3s 12      //
#define TILE_4s 13      //
#define TILE_5s 14      //
#define TILE_6s 15      //
#define TILE_7s 16      //
#define TILE_8s 17      //
#define TILE_9s 18      //
#define TILE_1p 19      //饼
#define TILE_2p 20      //
#define TILE_3p 21      //
#define TILE_4p 22      //
#define TILE_5p 23      //
#define TILE_6p 24      //
#define TILE_7p 25      //
#define TILE_8p 26      //
#define TILE_9p 27      //
#define TILE_E 28       //东南西北
#define TILE_S 29       //
#define TILE_W 30       //
#define TILE_N 31       //
#define TILE_C 32       //中发白
#define TILE_F 33       //
#define TILE_P 34       //
#define TILE_MEI 35     //花牌
#define TILE_LAN 36     //
#define TILE_ZHU 37     //
#define TILE_JU 38      //
#define TILE_CHU 39     //
#define TILE_XIA 40     //
#define TILE_QIU 41     //
#define TILE_DONG 42    //
#define TILE_BAIDA 43   //百搭牌
#define TILE_MAJIANG 44 //麻将背面
#define TILE_SIZE 43    //总有效张数（右开区间）

//花色
#define SUIT_INVALID 0 //无效花色
#define SUIT_WAN 1     //万子
#define SUIT_TIAO 2    //条子
#define SUIT_BING 3    //饼子
#define SUIT_HUA 4     //花牌
#define SUIT_FENG 5    //风牌
#define SUIT_JIAN 6    //箭牌

//点数
#define RANK_INVALID 0 //无效点数
#define RANK_1 1
#define RANK_2 2
#define RANK_3 3
#define RANK_4 4
#define RANK_5 5
#define RANK_6 6
#define RANK_7 7
#define RANK_8 8
#define RANK_9 9

//字符
#define TILE_CHAR_INVALID ' ' //无效字符
#define TILE_CHAR_WAN 'm'     //万
#define TILE_CHAR_TIAO 's'    //条
#define TILE_CHAR_BING 'p'    //饼
#define TILE_CHAR_E 'E'       //东
#define TILE_CHAR_S 'S'       //南
#define TILE_CHAR_W 'W'       //西
#define TILE_CHAR_N 'N'       //北
#define TILE_CHAR_C 'C'       //中
#define TILE_CHAR_F 'F'       //发
#define TILE_CHAR_P 'P'       //白
#define TILE_CHAR_MEI 'a'     //梅
#define TILE_CHAR_LAN 'b'     //兰
#define TILE_CHAR_ZHU 'c'     //竹
#define TILE_CHAR_JU 'd'      //菊
#define TILE_CHAR_CHU 'e'     //春
#define TILE_CHAR_XIA 'f'     //夏
#define TILE_CHAR_QIU 'g'     //秋
#define TILE_CHAR_DONG 'h'    //冬

//种类bitmap
#define BITMAP(t) (1ll << t)
#define TILE_TYPE_BITMAP_WAN (BITMAP(TILE_1m) | BITMAP(TILE_2m) | BITMAP(TILE_3m) | BITMAP(TILE_4m) | BITMAP(TILE_5m) | BITMAP(TILE_6m) | BITMAP(TILE_7m) | BITMAP(TILE_8m) | BITMAP(TILE_9m))                                                                                               //万
#define TILE_TYPE_BITMAP_TIAO (BITMAP(TILE_1s) | BITMAP(TILE_2s) | BITMAP(TILE_3s) | BITMAP(TILE_4s) | BITMAP(TILE_5s) | BITMAP(TILE_6s) | BITMAP(TILE_7s) | BITMAP(TILE_8s) | BITMAP(TILE_9s))                                                                                              //条
#define TILE_TYPE_BITMAP_BING (BITMAP(TILE_1p) | BITMAP(TILE_2p) | BITMAP(TILE_3p) | BITMAP(TILE_4p) | BITMAP(TILE_5p) | BITMAP(TILE_6p) | BITMAP(TILE_7p) | BITMAP(TILE_8p) | BITMAP(TILE_9p))                                                                                              //饼
#define TILE_TYPE_BITMAP_SHU (TILE_TYPE_BITMAP_WAN | TILE_TYPE_BITMAP_TIAO | TILE_TYPE_BITMAP_BING)                                                                                                                                                                                          //数牌
#define TILE_TYPE_BITMAP_FENG (BITMAP(TILE_E) | BITMAP(TILE_S) | BITMAP(TILE_W) | BITMAP(TILE_N))                                                                                                                                                                                            //风牌
#define TILE_TYPE_BITMAP_JIAN (BITMAP(TILE_C) | BITMAP(TILE_F) | BITMAP(TILE_P))                                                                                                                                                                                                             //箭牌
#define TILE_TYPE_BITMAP_ZI (TILE_TYPE_BITMAP_FENG | TILE_TYPE_BITMAP_JIAN)                                                                                                                                                                                                                  //字牌
#define TILE_TYPE_BITMAP_MEANINGFUL (TILE_TYPE_BITMAP_SHU | TILE_TYPE_BITMAP_ZI)                                                                                                                                                                                                             //数牌+字牌
#define TILE_TYPE_BITMAP_YAOJIU (TILE_TYPE_BITMAP_ZI | BITMAP(TILE_1m) | BITMAP(TILE_9m) | BITMAP(TILE_1s) | BITMAP(TILE_9s) | BITMAP(TILE_1p) | BITMAP(TILE_9p))                                                                                                                            //幺九牌
#define TILE_TYPE_BITMAP_LV (BITMAP(TILE_2s) | BITMAP(TILE_3s) | BITMAP(TILE_4s) | BITMAP(TILE_6s) | BITMAP(TILE_8s) | BITMAP(TILE_F))                                                                                                                                                       //绿一色牌
#define TILE_TYPE_BITMAP_QUANDA (BITMAP(TILE_7m) | BITMAP(TILE_8m) | BITMAP(TILE_9m) | BITMAP(TILE_7s) | BITMAP(TILE_8s) | BITMAP(TILE_9s) | BITMAP(TILE_7p) | BITMAP(TILE_8p) | BITMAP(TILE_9p))                                                                                            //全大
#define TILE_TYPE_BITMAP_QUANZHONG (BITMAP(TILE_4m) | BITMAP(TILE_5m) | BITMAP(TILE_6m) | BITMAP(TILE_4s) | BITMAP(TILE_5s) | BITMAP(TILE_6s) | BITMAP(TILE_4p) | BITMAP(TILE_5p) | BITMAP(TILE_6p))                                                                                         //全中
#define TILE_TYPE_BITMAP_QUANXIAO (BITMAP(TILE_1m) | BITMAP(TILE_2m) | BITMAP(TILE_3m) | BITMAP(TILE_1s) | BITMAP(TILE_2s) | BITMAP(TILE_3s) | BITMAP(TILE_1p) | BITMAP(TILE_2p) | BITMAP(TILE_3p))                                                                                          //全小
#define TILE_TYPE_BITMAP_DAYUWU (TILE_TYPE_BITMAP_QUANDA | BITMAP(TILE_6m) | BITMAP(TILE_6s) | BITMAP(TILE_6p))                                                                                                                                                                              //大于五
#define TILE_TYPE_BITMAP_XIAOYUWU (TILE_TYPE_BITMAP_QUANXIAO | BITMAP(TILE_4m) | BITMAP(TILE_4s) | BITMAP(TILE_4p))                                                                                                                                                                          //小于五
#define TILE_TYPE_BITMAP_TUIBUDAO (BITMAP(TILE_2s) | BITMAP(TILE_4s) | BITMAP(TILE_5s) | BITMAP(TILE_6s) | BITMAP(TILE_8s) | BITMAP(TILE_9s) | BITMAP(TILE_1p) | BITMAP(TILE_2p) | BITMAP(TILE_3p) | BITMAP(TILE_4p) | BITMAP(TILE_5p) | BITMAP(TILE_8p) | BITMAP(TILE_9p) | BITMAP(TILE_P)) //推不倒

extern const char *TILES_UTF8[];
extern const unsigned TILES_SUIT[];
extern const unsigned TILES_RANK[];
extern const char TILES_SUIT_CHAR[];

//牌类
class Tile {
  public:
    Tile(int t = TILE_INVALID, int _drawflag = 0) : _tile(t), _drawflag(_drawflag) {}
    Tile &operator=(int t) {
        _tile = t;
        ResetDrawflag();
        return *this;
    }
    Tile &operator=(const Tile &tile) {
        _tile = tile._tile;
        _drawflag = tile._drawflag;
        return *this;
    }
    bool operator==(const Tile &tile) const { return _tile == tile._tile; }
    bool operator==(int t) const { return _tile == t; }
    bool operator<(const Tile &tile) const { return _tile < tile._tile; }

    Tile Pred() const { return Tile(_tile - 1); }                              //返回前面一张牌
    Tile Succ() const { return Tile(_tile + 1); }                              //返回后面一张牌
    Tile GetTileUsingOffset(int offset) const { return Tile(_tile + offset); } //返回相距offset的牌

    int Suit() const { return TILES_SUIT[_tile]; } //返回牌的花色
    int Rank() const { return TILES_RANK[_tile]; } //返回牌的点数

    int IsShu() const { return (GetBitmap() & TILE_TYPE_BITMAP_SHU) == GetBitmap(); }       //是否为数牌
    int IsZi() const { return (GetBitmap() & TILE_TYPE_BITMAP_ZI) == GetBitmap(); }         //是否为字牌
    int IsFeng() const { return (GetBitmap() & TILE_TYPE_BITMAP_FENG) == GetBitmap(); }     //是否为风
    int IsJian() const { return (GetBitmap() & TILE_TYPE_BITMAP_JIAN) == GetBitmap(); }     //是否为箭牌
    int IsYaojiu() const { return (GetBitmap() & TILE_TYPE_BITMAP_YAOJIU) == GetBitmap(); } //是否为幺九牌
    int IsHua() const { return Suit() == SUIT_HUA; }                                        //是否为花牌

    const char *UTF8() const { return TILES_UTF8[_tile]; }              //返回麻将牌emoji的UTF-8字符串
    char RankChar() const { return '0' + Rank(); }                      //返回表示牌点数的字符（数牌为点数，其他为'0'）
    char SuitChar() const { return TILES_SUIT_CHAR[_tile]; }            //返回表示牌的花色字符（数牌为花色，其他为代表它的字符）
    char TileChar() const { return IsShu() ? RankChar() : SuitChar(); } //返回表示牌的字符（数牌为点数，其他为代表它的字符）

    void SetZimo() { _drawflag = 1; }       //设置自摸标记
    void SetChonghu() { _drawflag = 2; }    //设置铳和标记
    void ResetDrawflag() { _drawflag = 0; } //重置标记

    int IsZimo() const { return _drawflag == 1; }
    int IsChonghu() const { return _drawflag == 2; }

    int GetId() const { return _tile; }                  //返回牌的编号
    long long GetBitmap() const { return 1ll << _tile; } //返回牌的位图
    int GetDrawflag() const { return _drawflag; }        //返回抓牌标记

  private:
    int _tile;     //牌
    int _drawflag; //1表示自摸牌，2表示铳和牌（仅用于暗刻的判定），0表示之前正常抓上来的牌。仅在算番时标记
};

} // namespace mahjong

#endif
