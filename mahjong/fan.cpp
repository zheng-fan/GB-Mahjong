#include "fan.h"

namespace mahjong {

const int FAN_SCORE[] = {
    0,
    88, 88, 88, 88, 88, 88, 88,
    64, 64, 64, 64, 64, 64,
    48, 48,
    32, 32, 32,
    24, 24, 24, 24, 24, 24, 24, 24, 24,
    16, 16, 16, 16, 16, 16,
    12, 12, 12, 12, 12,
    8, 8, 8, 8, 8, 8, 8, 8, 8,
    6, 6, 6, 6, 6, 6, 6,
    4, 4, 4, 4,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    5};

const char *FAN_NAME[] = {
    "无效番种",
    "大四喜", "大三元", "绿一色", "九莲宝灯", "四杠", "连七对", "十三幺",
    "清幺九", "小四喜", "小三元", "字一色", "四暗刻", "一色双龙会",
    "一色四同顺", "一色四节高",
    "一色四步高", "三杠", "混幺九",
    "七对", "七星不靠", "全双刻", "清一色", "一色三同顺", "一色三节高", "全大", "全中", "全小",
    "清龙", "三色双龙会", "一色三步高", "全带五", "三同刻", "三暗刻",
    "全不靠", "组合龙", "大于五", "小于五", "三风刻",
    "花龙", "推不倒", "三色三同顺", "三色三节高", "无番和", "妙手回春", "海底捞月", "杠上开花", "抢杠和",
    "碰碰和", "混一色", "三色三步高", "五门齐", "全求人", "双暗杠", "双箭刻",
    "全带幺", "不求人", "双明杠", "和绝张",
    "箭刻", "圈风刻", "门风刻", "门前清", "平和", "四归一", "双同刻", "双暗刻", "暗杠", "断幺",
    "一般高", "喜相逢", "连六", "老少副", "幺九刻", "明杠", "缺一门", "无字", "边张", "坎张", "单钓将", "自摸", "花牌",
    "明暗杠"};

int Fan::_Judge2SameOrAdjacent(const Tile &a, const Tile &b) const {
    if (a.IsShu() && a.Suit() == b.Suit()) {
        return a == b.Pred() || a == b;
    } else if (a.IsZi()) {
        return a == b;
    }
    return 0;
}
int Fan::_Judge3MakePack(const Tile &a, const Tile &b, const Tile &c) const {
    if (a.IsShu() && b.Suit() == a.Suit() && b.Suit() == c.Suit() && b == a.Succ() && b == c.Pred()) {
        return PACK_TYPE_SHUNZI;
    } else if (b == a && b == c) {
        return PACK_TYPE_KEZI;
    }
    return 0;
}
int Fan::_Judge2MakePack(const Tile &a, const Tile &b) const {
    if (b == a) {
        return PACK_TYPE_JIANG;
    }
    return 0;
}

int Fan::_JudgeZuhelong(long long bitmap) const {
    for (int i = 1; i <= 6; i++) {
        if ((ZuhelongBitmap[i] & bitmap) == ZuhelongBitmap[i]) {
            return i;
        }
    }
    return 0;
}
int Fan::_JudgePartOfZuhelong(long long bitmap) const {
    bitmap &= TILE_TYPE_BITMAP_SHU;
    for (int i = 1; i <= 6; i++) {
        if ((ZuhelongBitmap[i] | bitmap) == ZuhelongBitmap[i]) {
            return 1;
        }
    }
    return 0;
}

int Fan::_BitCount(long long n) const {
    int c = 0;
    for (c = 0; n; ++c) {
        n &= (n - 1);
    }
    return c;
}

} // namespace mahjong
