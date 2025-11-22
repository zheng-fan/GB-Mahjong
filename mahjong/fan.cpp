#include "fan.h"
#include "console.h"
#include "print.h"
#include <chrono>

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

void Fan::CountFan(const Handtiles &ht) {
    //首先清空
    _Clear();
    //特殊和型后算番（要记录下当前的和型），接着再进行简单和型算番
    fan_t f;
    f = _JudgeCompleteSpecialHu(ht);
    int flag_quanbukao = 0;
    if (f) { //十三幺、全不靠、七星不靠
        flag_quanbukao = 1;
        _AddFan(f, {});
        _CountWinModeFan(ht, std::vector<Pack>(), Pack(), std::vector<Tile>());
        //全不靠、七星不靠可复合组合龙
        int zuhelong_type = _JudgeZuhelong(ht.LipaiBitmap());
        if (zuhelong_type) {
            fan_packs.push_back(Pack(PACK_TYPE_ZUHELONG, Tile(), zuhelong_type));
            _AddFan(FAN_ZUHELONG, {0});
        }
        //由于没有进行整体属性类算番，不需要手动去除五门齐
        // _ExcludeFan(FAN_WUMENQI, {});
        _ExcludeFan(FAN_BUQIUREN, {});
        _ExcludeFan(FAN_MENQIANQING, {});
        if (ht.IsZimo()) { //必然门前清的番种自摸和牌时不计求人，只计自摸
            fan_table[FAN_ZIMO].clear();
            excluded_fan_table[FAN_ZIMO].clear();
            fan_table[FAN_ZIMO].push_back({});
        }
        _GetMaxFan();
    }
    f = _JudgeQidui(ht);
    if (f) { //七对、连七对
        _AddFan(f, {});
        _CountOverallAttrFan(ht, std::vector<Pack>(), Pack());
        _CountWinModeFan(ht, std::vector<Pack>(), Pack(), std::vector<Tile>());
        _ExcludeFan(FAN_BUQIUREN, {});
        _ExcludeFan(FAN_MENQIANQING, {});
        if (f == FAN_LIANQIDUI) { //需要特判连七对需要排除的番种
            _ExcludeFan(FAN_QINGYISE, {});
            _ExcludeFan(FAN_WUZI, {});
        }
        if (ht.IsZimo()) { //必然门前清的番种自摸和牌时不计求人，只计自摸
            fan_table[FAN_ZIMO].clear();
            excluded_fan_table[FAN_ZIMO].clear();
            fan_table[FAN_ZIMO].push_back({});
        }
        _GetMaxFan();
    }
    std::vector<Tile> sorted_lipai;
    int zuhelong_type = _JudgeZuhelong(ht.LipaiBitmap());
    long long zuhelong_bitmap = ZuhelongBitmap[zuhelong_type];
    if (zuhelong_bitmap) { //若存在组合龙则需要去掉组合龙的牌进行后面的dfs
        long long bitmap_temp = zuhelong_bitmap;
        for (size_t i = 0; i < ht.lipai.size(); i++) {
            if (ht.lipai[i].GetBitmap() & bitmap_temp) {
                bitmap_temp ^= ht.lipai[i].GetBitmap();
            } else {
                sorted_lipai.push_back(ht.lipai[i]);
            }
        }
    } else {
        sorted_lipai = ht.lipai;
    }
    std::sort(sorted_lipai.begin(), sorted_lipai.end());
    std::vector<Pack> packs = ht.fulu;
    if (zuhelong_bitmap && !flag_quanbukao) {
        _Dfs(ht, sorted_lipai, 1 - ht.fulu.size(), 1, packs, 1, Pack(PACK_TYPE_ZUHELONG, Tile(), zuhelong_type));
        fan_packs_res.push_back(Pack(PACK_TYPE_ZUHELONG, Tile(), zuhelong_type));
        fan_table_res[FAN_ZUHELONG].push_back({(int)(fan_packs_res.size() - 1)});
        tot_fan_res += FAN_SCORE[FAN_ZUHELONG];
    } else {
#ifdef DEBUG_DFS_CNT
        _cnt_dfs = 0;
        _cnt_dfs_res = 0;
        _time_dfs_s = std::chrono::high_resolution_clock::now();
#endif
        _Dfs(ht, sorted_lipai, 4 - ht.fulu.size(), 1, packs, 1);
#ifdef DEBUG_DFS_CNT
        _time_dfs_e = std::chrono::high_resolution_clock::now();
        printf("_cnt_dfs=%d, _cnt_dfs_res=%d\n", _cnt_dfs, _cnt_dfs_res);
        printf("time_dfs=%.2fms, time_count_fan=%.2fms\n",
               std::chrono::duration<double, std::milli>(_time_dfs_e - _time_dfs_s).count(),
               std::chrono::duration<double, std::milli>(_time_count_fan_e - _time_count_fan_s).count());
#endif
    }
    //加上花牌
    int cnt_hua = ht.HuapaiCount();
    for (int i = 0; i < cnt_hua; i++) {
        fan_table_res[FAN_HUAPAI].push_back({});
    }
    tot_fan_res += cnt_hua;
}

void Fan::_ExcludeYaojiuke(const std::vector<Pack> &packs) {
    for (size_t i = 0; i < packs.size(); i++) {
        int rank = packs[i].GetMiddleTile().Rank();
        int is_zi = packs[i].GetMiddleTile().IsZi();
        if (packs[i].IsKeGang() && (rank == RANK_1 || rank == RANK_9 || is_zi)) {
            _ExcludeFan(FAN_YAOJIUKE, {(int)i});
        }
    }
}
void Fan::_CountOverallAttrFan(const Handtiles &ht, const std::vector<Pack> &packs, const Pack &zuhelong_pack) {
    long long bitmap = (ht.LipaiBitmap() | ht.FuluBitmap());
    if (zuhelong_pack.GetZuhelongType() == 0) {
        { //绿一色
            if ((bitmap & TILE_TYPE_BITMAP_LV) == bitmap) {
                _AddFan(FAN_LVYISE, {});
                _ExcludeFan(FAN_HUNYISE, {});
            }
        }
        { //九莲宝灯
            if (ht.NoFulu()) {
                std::unordered_map<int, int> tile_table = ht.lipai_table;
                tile_table[ht.GetLastLipai().GetId()]--;
                int st = tile_table[TILE_1m] ? TILE_1m : tile_table[TILE_1s] ? TILE_1s : TILE_1p;
                int flag = 1;
                for (int i = 2; i <= 8; i++) {
                    if (tile_table[st - 1 + i] != 1) {
                        flag = 0;
                        break;
                    }
                }
                if (tile_table[st] != 3 || tile_table[st + 8] != 3) {
                    flag = 0;
                }
                tile_table[ht.GetLastLipai().GetId()]++;
                if (flag) {
                    _AddFan(FAN_JIULIANBAODENG, {});
                    _ExcludeFan(FAN_QINGYISE, {});
                    _ExcludeFan(FAN_BUQIUREN, {});
                    _ExcludeFan(FAN_MENQIANQING, {});
                    _ExcludeFan(FAN_WUZI, {});
                    //减计一个幺九刻：和147只有一个幺九刻111，和369只有一个幺九刻999，和258有两个幺九刻。因此最终只有和258有一个幺九刻。
                    _ExcludeFan(FAN_YAOJIUKE, {int(std::find_if(packs.begin(), packs.end(), [](const Pack &p) -> int { return p.IsKezi() && p.GetMiddleTile().IsYaojiu(); }) - packs.begin())});
                }
            }
        }
        { //清幺九
            if ((bitmap & (TILE_TYPE_BITMAP_YAOJIU & (~TILE_TYPE_BITMAP_ZI))) == bitmap) {
                _AddFan(FAN_QINGYAOJIU, {});
                _ExcludeFan(FAN_PENGPENGHU, {});
                _ExcludeFan(FAN_QUANDAIYAO, {});
                _ExcludeFan(FAN_WUZI, {});
                for (size_t i = 0; i < packs.size(); i++) {
                    for (size_t j = i + 1; j < packs.size(); j++) {
                        if (packs[i].IsKeGang() && packs[j].IsKeGang() && packs[i].GetMiddleTile().Rank() == packs[j].GetMiddleTile().Rank()) {
                            _ExcludeFan(FAN_SHUANGTONGKE, {(int)i, (int)j});
                        }
                    }
                }
                _ExcludeYaojiuke(packs);
            }
        }
        { //字一色
            if ((bitmap & TILE_TYPE_BITMAP_ZI) == bitmap) {
                _AddFan(FAN_ZIYISE, {});
                _ExcludeFan(FAN_PENGPENGHU, {});
                _ExcludeFan(FAN_QUANDAIYAO, {});
                _ExcludeYaojiuke(packs);
            }
        }
        { //混幺九
            if ((bitmap & TILE_TYPE_BITMAP_YAOJIU & (~TILE_TYPE_BITMAP_ZI)) && (bitmap & TILE_TYPE_BITMAP_ZI) && (bitmap & TILE_TYPE_BITMAP_YAOJIU) == bitmap) {
                //必须有19、有字，且全为幺九牌
                _AddFan(FAN_HUNYAOJIU, {});
                _ExcludeFan(FAN_PENGPENGHU, {});
                _ExcludeFan(FAN_QUANDAIYAO, {});
                _ExcludeYaojiuke(packs);
            }
        }
        { //全双刻
            if (packs.size() == 5) {
                int flag = 1;
                for (auto p : packs) {
                    if (!((p.IsKeGang() || p.IsJiang()) && p.GetMiddleTile().IsShu() && p.GetMiddleTile().Rank() % 2 == 0)) {
                        flag = 0;
                        break;
                    }
                }
                if (flag) {
                    _AddFan(FAN_QUANSHUANGKE, {});
                    _ExcludeFan(FAN_PENGPENGHU, {});
                    _ExcludeFan(FAN_DUANYAO, {});
                    _ExcludeFan(FAN_WUZI, {});
                }
            }
        }
        { //清一色
            if ((bitmap & TILE_TYPE_BITMAP_WAN) == bitmap || (bitmap & TILE_TYPE_BITMAP_TIAO) == bitmap || (bitmap & TILE_TYPE_BITMAP_BING) == bitmap) {
                _AddFan(FAN_QINGYISE, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //全大
            if ((bitmap & TILE_TYPE_BITMAP_QUANDA) == bitmap) {
                _AddFan(FAN_QUANDA, {});
                _ExcludeFan(FAN_DAYUWU, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //全中
            if ((bitmap & TILE_TYPE_BITMAP_QUANZHONG) == bitmap) {
                _AddFan(FAN_QUANZHONG, {});
                _ExcludeFan(FAN_DUANYAO, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //全小
            if ((bitmap & TILE_TYPE_BITMAP_QUANXIAO) == bitmap) {
                _AddFan(FAN_QUANXIAO, {});
                _ExcludeFan(FAN_XIAOYUWU, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //全带五
            if (packs.size() == 5) {
                int flag = 1;
                for (auto p : packs) {
                    int rank = p.GetMiddleTile().Rank();
                    if (!((p.IsShunzi() && (RANK_4 <= rank && rank <= RANK_6)) || ((p.IsKeGang() || p.IsJiang()) && rank == RANK_5))) {
                        flag = 0;
                        break;
                    }
                }
                if (flag) {
                    _AddFan(FAN_QUANDAIWU, {});
                    _ExcludeFan(FAN_DUANYAO, {});
                    _ExcludeFan(FAN_WUZI, {});
                }
            }
        }
        { //大于五
            if ((bitmap & TILE_TYPE_BITMAP_DAYUWU) == bitmap) {
                _AddFan(FAN_DAYUWU, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //小于五
            if ((bitmap & TILE_TYPE_BITMAP_XIAOYUWU) == bitmap) {
                _AddFan(FAN_XIAOYUWU, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //推不倒
            if ((bitmap & TILE_TYPE_BITMAP_TUIBUDAO) == bitmap) {
                _AddFan(FAN_TUIBUDAO, {});
                _ExcludeFan(FAN_QUEYIMEN, {});
            }
        }
        { //碰碰和
            if (packs.size() == 5 && std::all_of(packs.begin(), packs.end(), [](const Pack &p) -> int { return p.IsKeGang() || p.IsJiang(); })) {
                _AddFan(FAN_PENGPENGHU, {});
            }
        }
        { //混一色
            long long bitmap_nozi = (bitmap & (~TILE_TYPE_BITMAP_ZI));
            if ((bitmap & TILE_TYPE_BITMAP_ZI) && (bitmap & TILE_TYPE_BITMAP_SHU) && ((bitmap_nozi & TILE_TYPE_BITMAP_WAN) == bitmap_nozi || (bitmap_nozi & TILE_TYPE_BITMAP_TIAO) == bitmap_nozi || (bitmap_nozi & TILE_TYPE_BITMAP_BING) == bitmap_nozi)) {
                _AddFan(FAN_HUNYISE, {});
            }
        }
        { //全带幺
            if (packs.size() == 5) {
                int flag = 1;
                for (auto p : packs) {
                    int rank = p.GetMiddleTile().Rank();
                    int is_yaojiu = p.GetMiddleTile().IsYaojiu();
                    if (!((p.IsShunzi() && (rank == RANK_2 || rank == RANK_8)) || ((p.IsKeGang() || p.IsJiang()) && is_yaojiu))) {
                        flag = 0;
                        break;
                    }
                }
                if (flag) {
                    _AddFan(FAN_QUANDAIYAO, {});
                }
            }
        }
        { //断幺
            if ((bitmap & (~TILE_TYPE_BITMAP_YAOJIU)) == bitmap) {
                _AddFan(FAN_DUANYAO, {});
                _ExcludeFan(FAN_WUZI, {});
            }
        }
        { //缺一门
            if (((bitmap & TILE_TYPE_BITMAP_WAN) != 0) + ((bitmap & TILE_TYPE_BITMAP_TIAO) != 0) + ((bitmap & TILE_TYPE_BITMAP_BING) != 0) == 2) {
                _AddFan(FAN_QUEYIMEN, {});
            }
        }
    }
    //组合龙也可以计的整体属性类番种
    { //五门齐
        if (((bitmap & TILE_TYPE_BITMAP_WAN) != 0) + ((bitmap & TILE_TYPE_BITMAP_TIAO) != 0) + ((bitmap & TILE_TYPE_BITMAP_BING) != 0) + ((bitmap & TILE_TYPE_BITMAP_FENG) != 0) + ((bitmap & TILE_TYPE_BITMAP_JIAN) != 0) == 5) {
            _AddFan(FAN_WUMENQI, {});
        }
    }
    { //平和
        if (packs.size() != 7 && packs.size() && std::all_of(packs.begin(), packs.end(), [](const Pack &p) -> int { return p.IsShunzi() || (p.IsJiang() && p.GetMiddleTile().IsShu()); })) {
            _AddFan(FAN_PINGHU, {});
            _ExcludeFan(FAN_WUZI, {});
        }
    }
    { //四归一
        for (int i = TILE_1m; i < TILE_SIZE; i++) {
            int flag = 0;
            for (auto p : packs) {
                if (p.IsGang() && p.GetMiddleTile() == i) {
                    flag = 1;
                    break;
                }
            }
            if (flag) { //杠不能算四归一
                continue;
            }
            if (ht.HandTileCount(i) == 4) {
                _AddFan(FAN_SIGUIYI, {});
            }
        }
    }
    { //无字
        if ((bitmap & (~TILE_TYPE_BITMAP_ZI)) == bitmap) {
            _AddFan(FAN_WUZI, {});
        }
    }
}
void Fan::_CountKeGangFan(const Handtiles &ht, const std::vector<Pack> &packs) {
    std::vector<int> angang, minggang, anke;
    for (size_t i = 0; i < packs.size(); i++) { //统计暗杠、明杠、暗刻信息
        if (packs[i].IsGang()) {
            if (packs[i].IsAnshou()) {
                angang.push_back(i);
            } else {
                minggang.push_back(i);
            }
        } else if (packs[i].IsKezi() && packs[i].IsAnshou()) {
            anke.push_back(i);
        }
    }
    switch (angang.size() * 100 + minggang.size() * 10 + anke.size()) {
    //多个番种的刻杠类
    case 400:
        _AddFan(FAN_SIGANG, angang);
        _AddFan(FAN_SIANKE, angang);
        break;
    case 310:
        _AddFan(FAN_SIGANG, {angang[0], angang[1], angang[2], minggang[0]});
        _AddFan(FAN_SANANKE, angang);
        break;
    case 220:
        _AddFan(FAN_SIGANG, {angang[0], angang[1], minggang[0], minggang[1]});
        _AddFan(FAN_SHUANGANKE, angang);
        break;
    case 130:
        _AddFan(FAN_SIGANG, {angang[0], minggang[0], minggang[1], minggang[2]});
        break;
    case 301:
        _AddFan(FAN_SANGANG, angang);
        _AddFan(FAN_SIANKE, {angang[0], angang[1], angang[2], anke[0]});
        break;
    case 300:
        _AddFan(FAN_SANGANG, angang);
        _AddFan(FAN_SANANKE, angang);
        break;
    case 211:
        _AddFan(FAN_SANGANG, {angang[0], angang[1], minggang[0]});
        _AddFan(FAN_SANANKE, {angang[0], angang[1], anke[0]});
        break;
    case 210:
        _AddFan(FAN_SANGANG, {angang[0], angang[1], minggang[0]});
        _AddFan(FAN_SHUANGANKE, {angang[0], angang[1]});
        break;
    case 121:
        _AddFan(FAN_SANGANG, {angang[0], minggang[0], minggang[1]});
        _AddFan(FAN_SHUANGANKE, {angang[0], anke[0]});
        break;
    case 120:
        _AddFan(FAN_SANGANG, {angang[0], minggang[0], minggang[1]});
        break;
    case 202:
        _AddFan(FAN_SHUANGANGANG, angang);
        _AddFan(FAN_SIANKE, {angang[0], angang[1], anke[0], anke[1]});
        break;
    case 201:
        _AddFan(FAN_SHUANGANGANG, angang);
        _AddFan(FAN_SANANKE, {angang[0], angang[1], anke[0]});
        break;
    case 112:
        _AddFan(FAN_MINGANGANG, {angang[0], minggang[0]});
        _AddFan(FAN_SANANKE, {angang[0], anke[0], anke[1]});
        break;
    case 111:
        _AddFan(FAN_MINGANGANG, {angang[0], minggang[0]});
        _AddFan(FAN_SHUANGANKE, {angang[0], anke[0]});
        break;
    case 22: //不能加前导零以防止被认为是八进制
        _AddFan(FAN_SHUANGMINGGANG, minggang);
        _AddFan(FAN_SHUANGANKE, anke);
        break;
    case 103:
        _AddFan(FAN_ANGANG, angang);
        _AddFan(FAN_SIANKE, {angang[0], anke[0], anke[1], anke[2]});
        break;
    case 102:
        _AddFan(FAN_ANGANG, angang);
        _AddFan(FAN_SANANKE, {angang[0], anke[0], anke[1]});
        break;
    case 101:
        _AddFan(FAN_ANGANG, angang);
        _AddFan(FAN_SHUANGANKE, {angang[0], anke[0]});
        break;
    case 13: //不能加前导零以防止被认为是八进制
        _AddFan(FAN_MINGGANG, minggang);
        _AddFan(FAN_SANANKE, anke);
        break;
    case 12: //不能加前导零以防止被认为是八进制
        _AddFan(FAN_MINGGANG, minggang);
        _AddFan(FAN_SHUANGANKE, anke);
        break;
    //单个番种的刻杠类
    default: {
        int cnt_angang = angang.size();
        int cnt_minggang = minggang.size();
        int cnt_anke = anke.size();
        if (cnt_minggang == 4) {
            _AddFan(FAN_SIGANG, minggang);
        } else if (cnt_anke == 4) {
            _AddFan(FAN_SIANKE, anke);
        } else if (cnt_minggang == 3) {
            _AddFan(FAN_SANGANG, minggang);
        } else if (cnt_anke == 3) {
            _AddFan(FAN_SANANKE, anke);
        } else if (cnt_angang == 2) {
            _AddFan(FAN_SHUANGANGANG, angang);
        } else if (cnt_minggang == 2) {
            _AddFan(FAN_SHUANGMINGGANG, minggang);
        } else if (cnt_anke == 2) {
            _AddFan(FAN_SHUANGANKE, anke);
        } else if (cnt_minggang == 1 && cnt_angang == 1) {
            _AddFan(FAN_MINGANGANG, {angang[0], minggang[0]});
        } else if (cnt_angang == 1) {
            _AddFan(FAN_ANGANG, angang);
        } else if (cnt_minggang == 1) {
            _AddFan(FAN_MINGGANG, minggang);
        }
        break;
    }
    }
    if (_HasFan(FAN_SIGANG)) {
        _ExcludeFan(FAN_PENGPENGHU, {});
        for (size_t i = 0; i < packs.size(); i++) {
            if (packs[i].IsJiang()) {
                _ExcludeFan(FAN_DANDIAOJIANG, {(int)i});
                break;
            }
        }
    }
    if (_HasFan(FAN_SHUANGANGANG)) {
        _ExcludeFan(FAN_SHUANGANKE, {fan_table[FAN_SHUANGANGANG][0]});
    }
    if (_HasFan(FAN_SIANKE)) {
        _ExcludeFan(FAN_PENGPENGHU, {});
        _ExcludeFan(FAN_BUQIUREN, {});
        _ExcludeFan(FAN_MENQIANQING, {});
    }
}
void Fan::_StoreFan(std::vector<std::pair<fan_t, std::vector<int>>> &e, fan_t f, const std::vector<int> &v) {
    e.push_back(std::make_pair(f, v));
}
void Fan::_CountAssociatedCombinationFan(const Handtiles &ht, const std::vector<Pack> &packs) {
    std::vector<std::pair<fan_t, std::vector<int>>> e;
    std::vector<int> shunzi_id, kegang_id, jiang_id;
    for (size_t i = 0; i < packs.size(); i++) {
        if (packs[i].IsShunzi()) {
            shunzi_id.push_back((int)i);
        } else if (packs[i].IsKeGang()) {
            kegang_id.push_back((int)i);
        } else if (packs[i].IsJiang()) {
            jiang_id.push_back((int)i);
        }
    }
    //先统计所有能组成的番种
    { //大四喜、小四喜、三风刻
        std::vector<int> feng_kegang, feng_jiang;
        for (size_t i = 0; i < packs.size(); i++) {
            if (packs[i].GetMiddleTile().IsFeng()) {
                packs[i].IsKeGang() ? feng_kegang.push_back((int)i) : feng_jiang.push_back((int)i);
            }
        }
        if (feng_kegang.size() == 4) {
            _StoreFan(e, FAN_DASIXI, feng_kegang);
        }
        if (feng_kegang.size() == 3 && feng_jiang.size() == 1) {
            _StoreFan(e, FAN_XIAOSIXI, {feng_kegang[0], feng_kegang[1], feng_kegang[2], feng_jiang[0]});
        }
        if (feng_kegang.size() == 3) {
            _StoreFan(e, FAN_SANFENGKE, {feng_kegang[0], feng_kegang[1], feng_kegang[2]});
        }
    }
    { //大三元、小三元、双箭刻
        std::vector<int> jian_kegang, jian_jiang;
        for (size_t i = 0; i < packs.size(); i++) {
            if (packs[i].GetMiddleTile().IsJian()) {
                packs[i].IsKeGang() ? jian_kegang.push_back((int)i) : jian_jiang.push_back((int)i);
            }
        }
        if (jian_kegang.size() == 3) {
            _StoreFan(e, FAN_DASANYUAN, jian_kegang);
        }
        if (jian_kegang.size() == 2 && jian_jiang.size() == 1) {
            _StoreFan(e, FAN_XIAOSANYUAN, {jian_kegang[0], jian_kegang[1], jian_jiang[0]});
        }
        if (jian_kegang.size() == 2) {
            _StoreFan(e, FAN_SHUANGJIANKE, {jian_kegang[0], jian_kegang[1]});
        }
    }
    { //一色双龙会、三色双龙会
        std::vector<int> shunzi_123, shunzi_789;
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            int rank = packs[shunzi_id[i]].GetMiddleTile().Rank();
            if (rank == RANK_2) {
                shunzi_123.push_back(shunzi_id[i]);
            } else if (rank == RANK_8) {
                shunzi_789.push_back(shunzi_id[i]);
            }
        }
        if (shunzi_123.size() == 2 && shunzi_789.size() == 2 && packs[jiang_id[0]].GetMiddleTile().Rank() == 5) {
            int suit_123_1 = packs[shunzi_123[0]].GetMiddleTile().Suit();
            int suit_123_2 = packs[shunzi_123[1]].GetMiddleTile().Suit();
            int suit_789_1 = packs[shunzi_789[0]].GetMiddleTile().Suit();
            int suit_789_2 = packs[shunzi_789[1]].GetMiddleTile().Suit();
            int suit_jiang = packs[jiang_id[0]].GetMiddleTile().Suit();
            if (suit_123_1 == suit_123_2 && suit_123_1 == suit_789_1 && suit_123_1 == suit_789_2 && suit_123_1 == suit_jiang) {
                _StoreFan(e, FAN_YISESHUANGLONGHUI, {shunzi_123[0], shunzi_123[1], shunzi_789[0], shunzi_789[1], jiang_id[0]});
            } else if ((suit_123_1 == suit_789_1 && suit_123_2 == suit_789_2 || suit_123_1 == suit_789_2 && suit_123_2 == suit_789_1) &&
                       suit_123_1 != suit_123_2 && suit_123_1 != suit_jiang && suit_123_2 != suit_jiang)
                _StoreFan(e, FAN_SANSESHUANGLONGHUI, {shunzi_123[0], shunzi_123[1], shunzi_789[0], shunzi_789[1], jiang_id[0]});
        }
    }
    { //一色四同顺、一色三同顺、一般高
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            for (size_t j = i + 1; j < shunzi_id.size(); j++) {
                if (packs[shunzi_id[i]] == packs[shunzi_id[j]]) {
                    _StoreFan(e, FAN_YIBANGAO, {shunzi_id[i], shunzi_id[j]});
                } else {
                    continue;
                }
                for (size_t k = j + 1; k < shunzi_id.size(); k++) {
                    if (packs[shunzi_id[j]] == packs[shunzi_id[k]]) {
                        _StoreFan(e, FAN_YISESANTONGSHUN, {shunzi_id[i], shunzi_id[j], shunzi_id[k]});
                    } else {
                        continue;
                    }
                    for (size_t l = k + 1; l < shunzi_id.size(); l++) {
                        if (packs[shunzi_id[k]] == packs[shunzi_id[l]]) {
                            _StoreFan(e, FAN_YISESITONGSHUN, {shunzi_id[i], shunzi_id[j], shunzi_id[k], shunzi_id[l]});
                        }
                    }
                }
            }
        }
    }
    { //一色四节高、一色三节高、三色三节高
#define GetRank(x) (packs[sorted_kegang[x]].GetMiddleTile().Rank())
#define GetSuit(x) (packs[sorted_kegang[x]].GetMiddleTile().Suit())
        std::vector<std::pair<int, int>> kegang_temp;
        for (size_t i = 0; i < kegang_id.size(); i++) {
            if (packs[kegang_id[i]].GetMiddleTile().IsShu()) {
                kegang_temp.push_back(std::make_pair(packs[kegang_id[i]].GetMiddleTile().Rank(), kegang_id[i]));
            }
        }
        sort(kegang_temp.begin(), kegang_temp.end());
        std::vector<int> sorted_kegang;
        for (auto pr : kegang_temp) {
            sorted_kegang.push_back(pr.second);
        }
        for (size_t i = 0; i < sorted_kegang.size(); i++) {
            for (size_t j = i + 1; j < sorted_kegang.size(); j++) {
                if (GetRank(i) != GetRank(j) - 1) {
                    continue;
                }
                for (size_t k = j + 1; k < sorted_kegang.size(); k++) {
                    if (GetRank(j) != GetRank(k) - 1) {
                        continue;
                    }
                    if (GetSuit(i) != GetSuit(j) && GetSuit(i) != GetSuit(k) && GetSuit(j) != GetSuit(k)) {
                        _StoreFan(e, FAN_SANSESANJIEGAO, {sorted_kegang[i], sorted_kegang[j], sorted_kegang[k]});
                    } else if (GetSuit(i) == GetSuit(j) && GetSuit(i) == GetSuit(k)) {
                        _StoreFan(e, FAN_YISESANJIEGAO, {sorted_kegang[i], sorted_kegang[j], sorted_kegang[k]});
                    } else {
                        continue;
                    }
                    for (size_t l = k + 1; l < sorted_kegang.size(); l++) {
                        if (GetSuit(i) == GetSuit(j) && GetSuit(i) == GetSuit(k) && GetSuit(i) == GetSuit(l)) {
                            _StoreFan(e, FAN_YISESIJIEGAO, {sorted_kegang[i], sorted_kegang[j], sorted_kegang[k], sorted_kegang[l]});
                        }
                    }
                }
            }
        }
#undef GetRank
#undef GetSuit
    }
    { //一色四步高、一色三步高、三色三步高
#define GetRank(x) (packs[sorted_shunzi[x]].GetMiddleTile().Rank())
#define GetSuit(x) (packs[sorted_shunzi[x]].GetMiddleTile().Suit())
        std::vector<std::pair<int, int>> shunzi_temp;
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            shunzi_temp.push_back(std::make_pair(packs[shunzi_id[i]].GetMiddleTile().Rank(), shunzi_id[i]));
        }
        sort(shunzi_temp.begin(), shunzi_temp.end());
        std::vector<int> sorted_shunzi;
        for (auto pr : shunzi_temp) {
            sorted_shunzi.push_back(pr.second);
        }
        for (size_t i = 0; i < sorted_shunzi.size(); i++) { //*一色四步高、一色三步高
            for (size_t j = i + 1; j < sorted_shunzi.size(); j++) {
                int step_1 = GetRank(j) - GetRank(i);
                if ((step_1 != 1 && step_1 != 2) || GetSuit(i) != GetSuit(j)) {
                    continue;
                }
                for (size_t k = j + 1; k < sorted_shunzi.size(); k++) {
                    int step_2 = GetRank(k) - GetRank(j);
                    if ((step_2 != 1 && step_2 != 2) || GetSuit(j) != GetSuit(k)) {
                        continue;
                    }
                    if (step_1 == step_2) {
                        _StoreFan(e, FAN_YISESANBUGAO, {sorted_shunzi[i], sorted_shunzi[j], sorted_shunzi[k]});
                    }
                    for (size_t l = k + 1; l < sorted_shunzi.size(); l++) {
                        int step_3 = GetRank(l) - GetRank(k);
                        if ((step_3 != 1 && step_3 != 2) || GetSuit(k) != GetSuit(l)) {
                            continue;
                        }
                        if (step_1 == step_2 && step_1 == step_3) {
                            _StoreFan(e, FAN_YISESIBUGAO, {sorted_shunzi[i], sorted_shunzi[j], sorted_shunzi[k], sorted_shunzi[l]});
                        }
                    }
                }
            }
        }
        for (size_t i = 0; i < sorted_shunzi.size(); i++) { //*三色三步高
            for (size_t j = i + 1; j < sorted_shunzi.size(); j++) {
                if (GetRank(j) - GetRank(i) != 1 || GetSuit(i) == GetSuit(j)) {
                    continue;
                }
                for (size_t k = j + 1; k < sorted_shunzi.size(); k++) {
                    if (GetRank(k) - GetRank(j) != 1 || GetSuit(i) == GetSuit(k) || GetSuit(j) == GetSuit(k)) {
                        continue;
                    }
                    _StoreFan(e, FAN_SANSESANBUGAO, {sorted_shunzi[i], sorted_shunzi[j], sorted_shunzi[k]});
                }
            }
        }
#undef GetRank
#undef GetSuit
    }
    { //清龙、花龙
        std::vector<int> rank[10];
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            rank[packs[shunzi_id[i]].GetMiddleTile().Rank()].push_back(shunzi_id[i]);
        }
        if (rank[2].size() >= 1 && rank[5].size() >= 1 && rank[8].size() >= 1) {
            for (size_t i = 0; i < rank[2].size(); i++) {
                for (size_t j = 0; j < rank[5].size(); j++) {
                    for (size_t k = 0; k < rank[8].size(); k++) {
                        int suit_1 = packs[rank[2][i]].GetMiddleTile().Suit();
                        int suit_2 = packs[rank[5][j]].GetMiddleTile().Suit();
                        int suit_3 = packs[rank[8][k]].GetMiddleTile().Suit();
                        if (suit_1 == suit_2 && suit_1 == suit_3) {
                            _StoreFan(e, FAN_QINGLONG, {rank[2][i], rank[5][j], rank[8][k]});
                        }
                        if (suit_1 != suit_2 && suit_1 != suit_3 && suit_2 != suit_3) {
                            _StoreFan(e, FAN_HUALONG, {rank[2][i], rank[5][j], rank[8][k]});
                        }
                    }
                }
            }
        }
    }
    { //三同刻、双同刻
#define GetRank(x) (packs[kegang_id[x]].GetMiddleTile().Rank())
        for (size_t i = 0; i < kegang_id.size(); i++) {
            for (size_t j = i + 1; j < kegang_id.size(); j++) {
                if (packs[kegang_id[i]].GetMiddleTile().IsShu() && GetRank(i) == GetRank(j)) {
                    _StoreFan(e, FAN_SHUANGTONGKE, {kegang_id[i], kegang_id[j]});
                } else {
                    continue;
                }
                for (size_t k = j + 1; k < kegang_id.size(); k++) {
                    if (GetRank(j) == GetRank(k)) {
                        _StoreFan(e, FAN_SANTONGKE, {kegang_id[i], kegang_id[j], kegang_id[k]});
                    } else {
                        continue;
                    }
                }
            }
        }
#undef GetRank
    }
    { //三色三同顺
#define GetRank(x) (packs[shunzi_id[x]].GetMiddleTile().Rank())
#define GetSuit(x) (packs[shunzi_id[x]].GetMiddleTile().Suit())
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            for (size_t j = i + 1; j < shunzi_id.size(); j++) {
                if (GetRank(i) != GetRank(j) || GetSuit(i) == GetSuit(j)) {
                    continue;
                }
                for (size_t k = j + 1; k < shunzi_id.size(); k++) {
                    if (GetRank(j) == GetRank(k) && GetSuit(i) != GetSuit(k) && GetSuit(j) != GetSuit(k)) {
                        _StoreFan(e, FAN_SANSESANTONGSHUN, {shunzi_id[i], shunzi_id[j], shunzi_id[k]});
                    }
                }
            }
        }
#undef GetRank
#undef GetSuit
    }
    { //喜相逢、连六、老少副
#define GetRank(x) (packs[shunzi_id[x]].GetMiddleTile().Rank())
#define GetSuit(x) (packs[shunzi_id[x]].GetMiddleTile().Suit())
        for (size_t i = 0; i < shunzi_id.size(); i++) {
            for (size_t j = i + 1; j < shunzi_id.size(); j++) {
                if (GetSuit(i) != GetSuit(j)) {
                    if (GetRank(i) == GetRank(j)) {
                        _StoreFan(e, FAN_XIXIANGFENG, {shunzi_id[i], shunzi_id[j]});
                    }
                } else {
                    if (GetRank(i) == GetRank(j) + 3 || GetRank(i) == GetRank(j) - 3) {
                        _StoreFan(e, FAN_LIANLIU, {shunzi_id[i], shunzi_id[j]});
                    } else if (GetRank(i) == GetRank(j) + 6 || GetRank(i) == GetRank(j) - 6) {
                        _StoreFan(e, FAN_LAOSHAOFU, {shunzi_id[i], shunzi_id[j]});
                    }
                }
            }
        }
#undef GetRank
#undef GetSuit
    }
#ifdef DEBUG_ASSCOM_TABLE
    for (auto &pr : e) {
        printf("%s:", FAN_NAME[pr.first]);
        for (auto i : pr.second) {
            printf(" %s", PackToEmojiString(packs[i]).c_str());
        }
        putchar('\n');
    }
#endif
    //再进行判断能够不重复组成的番种
    /* 
         * 核心原则：不相同原则、不拆移原则、套算一次原则
         * + 不相同原则：凡已组合过某一番种的牌，不能再同其他一副牌组成相同的番种计分。
         * + 不拆移原则：确定一个番种后，不能将其自身再拆开互相组成新的番种计分。
         * + 套算一次原则：如有尚未组合过的一副牌，只可同已组合过的相应的一副牌套算一次。
         * 上述三个原则可统一理解为：面子与面子构成多种“关联组合类番种”时，组合关系不得形成循环。
         * 此处的算法是：使用并查集维护当前关联组合的状态，用哈希值代表状态，进行BFS搜索
         * 为了保证状态间哈希值相同，并查集中的合并规则必须相同，且必须合并到根
         */
    struct _Status {
        int f[5];
        std::vector<int> eid;
        int fan_cnt;
        _Status() {
            for (int i = 0; i < 5; i++) {
                f[i] = i;
            }
            fan_cnt = 0;
        }
        int find(int x) {
            return f[x] == x ? x : f[x] = find(f[x]);
        }
        void uni(int a, int b) {
            a = find(a);
            b = find(b);
            if (a == b)
                return;
            if (a < b) {
                f[b] = a;
            } else {
                f[a] = b;
            }
        }
        int uni(int id, const std::vector<std::pair<fan_t, std::vector<int>>> &e) {
            const std::vector<int> &v = e[id].second;
            for (size_t i = 0; i < v.size(); i++) {
                for (size_t j = i + 1; j < v.size(); j++) {
                    if (find(v[i]) == find(v[j])) {
                        return false;
                    }
                }
            }
            for (size_t i = 1; i < v.size(); i++) {
                uni(v[i], v[i - 1]);
            }
            eid.push_back(id);
            fan_cnt += FAN_SCORE[e[id].first];
            return true;
        }
        int hash() {
            int ret = 0;
            for (int i = 0; i < 5; i++) {
                ret *= 5;
                ret += find(i);
            }
            return ret;
        }
    };
    std::unordered_map<int, int> mp;
    int fan_cnt = 0;
    _Status mx; //最终番数最多的结果
    std::queue<_Status> q;
    q.push(_Status());
    while (!q.empty()) {
        _Status s_front = q.front();
        q.pop();
        for (size_t i = 0; i < e.size(); i++) {
            _Status s = s_front;
            if (s.uni(i, e) && s.fan_cnt > mp[s.hash()]) {
                q.push(s);
                mp[s.hash()] = s.fan_cnt;
                if (s.fan_cnt > fan_cnt) {
                    fan_cnt = s.fan_cnt;
                    mx = s;
                }
            }
        }
    }
    for (auto id : mx.eid) {
        _AddFan(e[id].first, e[id].second);
        switch (e[id].first) {
        case FAN_DASIXI:
            _ExcludeFan(FAN_PENGPENGHU, {});
            for (auto i : e[id].second) {
                if (packs[i].GetMiddleTile().IsFeng()) {
                    _ExcludeFan(FAN_QUANFENGKE, {i});
                    _ExcludeFan(FAN_MENFENGKE, {i});
                    _ExcludeFan(FAN_YAOJIUKE, {i});
                }
            }
            break;
        case FAN_DASANYUAN:
        case FAN_XIAOSANYUAN:
        case FAN_SHUANGJIANKE:
            for (auto i : e[id].second) {
                if (packs[i].GetMiddleTile().IsJian()) {
                    _ExcludeFan(FAN_JIANKE, {i});
                    _ExcludeFan(FAN_YAOJIUKE, {i});
                }
            }
            break;
        case FAN_XIAOSIXI:
        case FAN_SANFENGKE:
            for (auto i : e[id].second) {
                if (packs[i].GetMiddleTile().IsFeng()) {
                    _ExcludeFan(FAN_YAOJIUKE, {i});
                }
            }
            break;
        case FAN_YISESHUANGLONGHUI:
            _ExcludeFan(FAN_QINGYISE, {});
            _ExcludeFan(FAN_PINGHU, {});
            _ExcludeFan(FAN_WUZI, {});
            break;
        case FAN_YISESITONGSHUN:
            _ExcludeFan(FAN_SIGUIYI, {});
            _ExcludeFan(FAN_SIGUIYI, {});
            _ExcludeFan(FAN_SIGUIYI, {});
            break;
        case FAN_YISESIJIEGAO:
            _ExcludeFan(FAN_PENGPENGHU, {});
            break;
        case FAN_SANSESHUANGLONGHUI:
            _ExcludeFan(FAN_PINGHU, {});
            break;
        //其他番种无需再排除番种
        default:
            break;
        }
    }
}
void Fan::_CountSinglePackFan(const Handtiles &ht, const std::vector<Pack> &packs) {
    for (size_t i = 0; i < packs.size(); i++) {
        const Pack &p = packs[i];
        if (p.IsKeGang()) {
            if (p.GetMiddleTile().IsJian()) { //箭刻
                _AddFan(FAN_JIANKE, {(int)i});
                _ExcludeFan(FAN_YAOJIUKE, {(int)i});
            }
            if (p.GetMiddleTile() == ht.GetQuanfeng()) { //圈风刻
                _AddFan(FAN_QUANFENGKE, {(int)i});
                _ExcludeFan(FAN_YAOJIUKE, {(int)i});
            }
            if (p.GetMiddleTile() == ht.GetMenfeng()) { //门风刻
                _AddFan(FAN_MENFENGKE, {(int)i});
                _ExcludeFan(FAN_YAOJIUKE, {(int)i});
            }
            if (p.GetMiddleTile().IsYaojiu()) { //幺九刻
                _AddFan(FAN_YAOJIUKE, {(int)i});
            }
        }
    }
}
void Fan::_CountWinModeFan(const Handtiles &ht, const std::vector<Pack> &packs, const Pack &zuhelong_pack, const std::vector<Tile> &ting) {
    { //妙手回春
        if (ht.IsHaidi() && ht.IsZimo()) {
            _AddFan(FAN_MIAOSHOUHUICHUN, {});
            _ExcludeFan(FAN_ZIMO, {});
        }
    }
    { //海底捞月
        if (ht.IsHaidi() && !ht.IsZimo()) {
            _AddFan(FAN_HAIDILAOYUE, {});
        }
    }
    { //杠上开花
        if (ht.IsGang() && ht.IsZimo()) {
            _AddFan(FAN_GANGSHANGKAIHUA, {});
            _ExcludeFan(FAN_ZIMO, {});
        }
    }
    { //抢杠和
        if (ht.IsGang() && !ht.IsZimo()) {
            _AddFan(FAN_QIANGGANGHU, {});
            _ExcludeFan(FAN_HUJUEZHANG, {});
        }
    }
    { //全求人
        if (ht.IsTotallyFulu() && !ht.IsZimo()) {
            _AddFan(FAN_QUANQIUREN, {});
            //找到将的packs编号
            _ExcludeFan(FAN_DANDIAOJIANG, {int(std::find_if(packs.begin(), packs.end(), [](const Pack &p) -> int { return p.IsJiang(); }) - packs.begin())});
        }
    }
    { //不求人
        if (ht.IsMenqing() && ht.IsZimo()) {
            _AddFan(FAN_BUQIUREN, {});
            _ExcludeFan(FAN_MENQIANQING, {});
            _ExcludeFan(FAN_ZIMO, {});
        }
    }
    { //和绝张
        if (ht.IsJuezhang()) {
            _AddFan(FAN_HUJUEZHANG, {});
            //找到将的packs编号
            _ExcludeFan(FAN_DANDIAOJIANG, {int(std::find_if(packs.begin(), packs.end(), [](const Pack &p) -> int { return p.IsJiang(); }) - packs.begin())});
        }
    }
    { //门前清
        if (ht.IsMenqing()) {
            _AddFan(FAN_MENQIANQING, {});
        }
    }
    { //边张、坎张、单钓将
        if (ting.size() == 1 && (zuhelong_pack.GetZuhelongBitmap() & ht.GetLastLipai().GetBitmap()) == 0) {
            for (size_t i = 0; i < packs.size(); i++) {
                const Pack &p = packs[i];
                const Tile &t = p.GetMiddleTile();
                if (p.HaveLastTile()) {
                    if (p.IsJiang()) {
                        _AddFan(FAN_DANDIAOJIANG, {(int)i}); //单钓将
                    } else if (p.IsShunzi()) {
                        if ((t.Rank() == RANK_2 && ht.GetLastLipai().Rank() == RANK_3) ||
                            (t.Rank() == RANK_8 && ht.GetLastLipai().Rank() == RANK_7)) {
                            _AddFan(FAN_BIANZHANG, {(int)i}); //边张
                        } else if (t.Rank() == ht.GetLastLipai().Rank()) {
                            _AddFan(FAN_KANZHANG, {(int)i}); //坎张
                        }
                    }
                    break;
                }
            }
        }
    }
    { //自摸
        if (ht.IsZimo()) {
            _AddFan(FAN_ZIMO, {});
            //九莲宝灯、四暗刻不计不求人但记自摸
            if ((_HasFan(FAN_JIULIANBAODENG) || _HasFan(FAN_SIANKE)) && !_HasFan(FAN_MIAOSHOUHUICHUN) && !_HasFan(FAN_GANGSHANGKAIHUA)) {
                excluded_fan_table[FAN_ZIMO].clear();
            }
        }
    }
}

fan_t Fan::_JudgeCompleteSpecialHu(const Handtiles &ht) {
    if (!ht.NoFulu()) {
        return fan_t(FAN_INVALID);
    }
    long long bitmap = ht.LipaiBitmap();
    int cnt = _BitCount(bitmap);

    //十三幺
    if ((bitmap & TILE_TYPE_BITMAP_YAOJIU) == bitmap && cnt == 13) {
        return FAN_SHISANYAO;
    }
    //全不靠、七星不靠
    if (_JudgePartOfZuhelong(bitmap) && ((bitmap & TILE_TYPE_BITMAP_MEANINGFUL) == bitmap) && cnt == 14) {
        if ((bitmap & TILE_TYPE_BITMAP_ZI) == TILE_TYPE_BITMAP_ZI) {
            return FAN_QIXINGBUKAO;
        } else {
            return FAN_QUANBUKAO;
        }
    }

    return fan_t(FAN_INVALID);
}
fan_t Fan::_JudgeQidui(const Handtiles &ht) {
    if (!ht.NoFulu()) {
        return fan_t(FAN_INVALID);
    }
    std::vector<Tile> sorted_lipai = ht.lipai;
    std::sort(sorted_lipai.begin(), sorted_lipai.end());
    std::vector<Pack> packs;
    int ret = _Dfs(ht, sorted_lipai, 0, 7, packs, 0);
    if (ret) {
        int flag = 1;
        for (size_t i = 1; i < packs.size(); i++) {
            if (!(packs[i - 1].GetMiddleTile().Succ() == packs[i].GetMiddleTile() && packs[i - 1].GetMiddleTile().Suit() == packs[i].GetMiddleTile().Suit())) {
                flag = 0;
                break;
            }
        }
        if (flag == 0) {
            return FAN_QIDUI;
        } else {
            return FAN_LIANQIDUI;
        }
    }
    return fan_t(FAN_INVALID);
}
int Fan::_JudgeBasicHu(const Handtiles &ht) {
    std::vector<Tile> sorted_lipai = ht.lipai;
    std::sort(sorted_lipai.begin(), sorted_lipai.end());
    std::vector<Pack> packs = ht.fulu;
    return _Dfs(ht, sorted_lipai, 4 - ht.fulu.size(), 1, packs, 0);
}
int Fan::_JudgeZuhelongBasicHu(const Handtiles &ht) {
    std::vector<Tile> sorted_lipai;
    int zuhelong_bitmap = ZuhelongBitmap[_JudgeZuhelong(ht.LipaiBitmap())];
    if (zuhelong_bitmap) {
        long long bitmap_temp = zuhelong_bitmap;
        for (size_t i = 0; i < ht.lipai.size(); i++) {
            if (ht.lipai[i].GetBitmap() & bitmap_temp) {
                bitmap_temp ^= ht.lipai[i].GetBitmap();
            } else {
                sorted_lipai.push_back(ht.lipai[i]);
            }
        }
        std::sort(sorted_lipai.begin(), sorted_lipai.end());
        std::vector<Pack> packs = ht.fulu;
        return _Dfs(ht, sorted_lipai, 1 - ht.fulu.size(), 1, packs, 0);
    }
    return 0;
}

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
