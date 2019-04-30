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

int Fan::_Dfs(const Handtiles &ht, const std::vector<Tile> &sorted_lipai, int mianzi_cnt, int duizi_cnt, std::vector<Pack> &packs, int flag_count_fan, const Pack &zuhelong_pack) {
    std::unordered_set<long long> st; //记录当前组成的packs是否已访问过，用来剪枝
    st.clear();
    std::vector<int> vis(14, 0);
    return _Dfs_recursive(ht, sorted_lipai, mianzi_cnt, duizi_cnt, vis, packs, flag_count_fan, zuhelong_pack, st);
}
int Fan::_Dfs_recursive(const Handtiles &ht, const std::vector<Tile> &sorted_lipai, int mianzi_cnt, int duizi_cnt, std::vector<int> &vis, std::vector<Pack> &packs, int flag_count_fan, const Pack &zuhelong_pack, std::unordered_set<long long> &st) {
#ifdef DEBUG_DFS
    StdPrintTile(ht.GetLastLipai());
    printf("dfs: %ld packs, duizicnt=%d,mianzicnt=%d\n", packs.size(), duizi_cnt, mianzi_cnt);
#endif
#ifdef DEBUG_DFS_CNT
    _cnt_dfs++;
#endif
    int ret = 0;
    if (mianzi_cnt == 0 && duizi_cnt == 0) {
        if (flag_count_fan) {
#ifdef DEBUG_DFS_CNT
            _time_count_fan_s = std::chrono::high_resolution_clock::now();
#endif
            _CountBasicFan(ht, packs, zuhelong_pack);
            fan_packs = packs;
            _GetMaxFan();
#ifdef DEBUG_DFS_CNT
            _cnt_dfs_res++;
            _time_count_fan_e = std::chrono::high_resolution_clock::now();
#endif
        }
        return 1;
    }
    int start_pos = -1;
    int n = sorted_lipai.size();
    for (int i = 0; i < n; i++) {
        if (vis[i] == 0) {
            start_pos = i;
            break;
        }
    }
    if (start_pos == -1) {
        return 0;
    }
    for (int i = start_pos + 1; i < n; i++) {
        if (vis[i]) {
            continue;
        }
        if (!_Judge2SameOrAdjacent(sorted_lipai[start_pos], sorted_lipai[i])) {
            break;
        }
        if (duizi_cnt && _Judge2MakePack(sorted_lipai[start_pos], sorted_lipai[i])) {
            vis[start_pos] = vis[i] = 1;
            int offer = -(sorted_lipai[start_pos].GetDrawflag() + sorted_lipai[i].GetDrawflag()); //立牌的drawflag只可能有一个非0，所以可以+=
            packs.push_back(Pack(PACK_TYPE_JIANG, sorted_lipai[i], 0, offer));
            long long hashcode = _PacksHashcode(ht, packs);
            if (st.find(hashcode) == st.end()) { //剪枝
                st.insert(hashcode);
#ifdef DEBUG_DFS
                StdPrintTile(ht.GetLastLipai());
                printf("dfs: %ld packs, duizicnt=%d, mianzicnt=%d, %d %d offer=%d\n", packs.size(), duizi_cnt, mianzi_cnt, start_pos, i, offer);
#endif
                ret |= _Dfs_recursive(ht, sorted_lipai, mianzi_cnt, duizi_cnt - 1, vis, packs, flag_count_fan, zuhelong_pack, st);
                if (flag_count_fan == 0 && ret) {
                    return 1;
                }
            }
            packs.erase(--packs.end());
            vis[start_pos] = vis[i] = 0;
        }
        if (mianzi_cnt) {
            for (int j = i + 1; j < n; j++) {
                if (vis[j]) {
                    continue;
                }
                if (!_Judge2SameOrAdjacent(sorted_lipai[i], sorted_lipai[j])) {
                    break;
                }
                int type = _Judge3MakePack(sorted_lipai[start_pos], sorted_lipai[i], sorted_lipai[j]);
                if (type) {
                    vis[start_pos] = vis[i] = vis[j] = 1;
                    int offer = -(sorted_lipai[start_pos].GetDrawflag() + sorted_lipai[i].GetDrawflag() + sorted_lipai[j].GetDrawflag()); //立牌的drawflag只可能有一个非0，所以可以+=
                    packs.push_back(Pack(type, sorted_lipai[i], 0, offer));
                    long long hashcode = _PacksHashcode(ht, packs);
                    if (st.find(hashcode) == st.end()) { //剪枝
                        st.insert(hashcode);
#ifdef DEBUG_DFS
                        StdPrintTile(ht.GetLastLipai());
                        printf("dfs: %ld packs, duizicnt=%d, mianzicnt=%d, %d %d %d offer=%d\n", packs.size(), duizi_cnt, mianzi_cnt, start_pos, i, j, offer);
#endif
                        ret |= _Dfs_recursive(ht, sorted_lipai, mianzi_cnt - 1, duizi_cnt, vis, packs, flag_count_fan, zuhelong_pack, st);
                        if (flag_count_fan == 0 && ret) {
                            return 1;
                        }
                    }
                    packs.erase(--packs.end());
                    vis[start_pos] = vis[i] = vis[j] = 0;
                }
            }
        }
    }
    return ret;
}
long long Fan::_PacksHashcode(const Handtiles &ht, const std::vector<Pack> &packs) {
    long long h = 0;
    //一个Pack需要11位，5个pack最多55位
    for (size_t i = ht.fulu.size(); i < packs.size(); i++) {
        const Pack &p = packs[i];
        //Middle Tile Id: 7 bits (Max Id = 44)
        //Pack Type Id: 3 bits (Max Id = 5)
        //Last Tile Flag: 1 bit
        h = ((h << 7 | p.GetMiddleTile().GetId()) << 3 | p.GetType()) << 1 | (p.HaveLastTile());
    }
    return h;
}

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
