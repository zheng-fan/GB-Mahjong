#ifndef __MAHJONG_FAN_H__
#define __MAHJONG_FAN_H__

#include "debug.h"
#include "handtiles.h"
#include "pack.h"
#include "tile.h"
#include <queue>
#include <unordered_set>
#include <vector>

namespace mahjong {

//番种
enum fan_t {
    FAN_INVALID = 0,        //无效番种
                            //========88番========
    FAN_DASIXI,             //大四喜
    FAN_DASANYUAN,          //大三元
    FAN_LVYISE,             //绿一色
    FAN_JIULIANBAODENG,     //九莲宝灯
    FAN_SIGANG,             //四杠
    FAN_LIANQIDUI,          //连七对
    FAN_SHISANYAO,          //十三幺
                            //========64番========
    FAN_QINGYAOJIU,         //清幺九
    FAN_XIAOSIXI,           //小四喜
    FAN_XIAOSANYUAN,        //小三元
    FAN_ZIYISE,             //字一色
    FAN_SIANKE,             //四暗刻
    FAN_YISESHUANGLONGHUI,  //一色双龙会
                            //========48番========
    FAN_YISESITONGSHUN,     //一色四同顺
    FAN_YISESIJIEGAO,       //一色四节高
                            //========32番========
    FAN_YISESIBUGAO,        //一色四步高
    FAN_SANGANG,            //三杠
    FAN_HUNYAOJIU,          //混幺九
                            //========24番========
    FAN_QIDUI,              //七对
    FAN_QIXINGBUKAO,        //七星不靠
    FAN_QUANSHUANGKE,       //全双刻
    FAN_QINGYISE,           //清一色
    FAN_YISESANTONGSHUN,    //一色三同顺
    FAN_YISESANJIEGAO,      //一色三节高
    FAN_QUANDA,             //全大
    FAN_QUANZHONG,          //全中
    FAN_QUANXIAO,           //全小
                            //========16番========
    FAN_QINGLONG,           //清龙
    FAN_SANSESHUANGLONGHUI, //三色双龙会
    FAN_YISESANBUGAO,       //一色三步高
    FAN_QUANDAIWU,          //全带五
    FAN_SANTONGKE,          //三同刻
    FAN_SANANKE,            //三暗刻
                            //========12番========
    FAN_QUANBUKAO,          //全不靠
    FAN_ZUHELONG,           //组合龙
    FAN_DAYUWU,             //大于五
    FAN_XIAOYUWU,           //小于五
    FAN_SANFENGKE,          //三风刻
                            //========8番========
    FAN_HUALONG,            //花龙
    FAN_TUIBUDAO,           //推不倒
    FAN_SANSESANTONGSHUN,   //三色三同顺
    FAN_SANSESANJIEGAO,     //三色三节高
    FAN_WUFANHU,            //无番和
    FAN_MIAOSHOUHUICHUN,    //妙手回春
    FAN_HAIDILAOYUE,        //海底捞月
    FAN_GANGSHANGKAIHUA,    //杠上开花
    FAN_QIANGGANGHU,        //抢杠和
                            //========6番========
    FAN_PENGPENGHU,         //碰碰和
    FAN_HUNYISE,            //混一色
    FAN_SANSESANBUGAO,      //三色三步高
    FAN_WUMENQI,            //五门齐
    FAN_QUANQIUREN,         //全求人
    FAN_SHUANGANGANG,       //双暗杠
    FAN_SHUANGJIANKE,       //双箭刻
                            //========4番========
    FAN_QUANDAIYAO,         //全带幺
    FAN_BUQIUREN,           //不求人
    FAN_SHUANGMINGGANG,     //双明杠
    FAN_HUJUEZHANG,         //和绝张
                            //========2番========
    FAN_JIANKE,             //箭刻
    FAN_QUANFENGKE,         //圈风刻
    FAN_MENFENGKE,          //门风刻
    FAN_MENQIANQING,        //门前清
    FAN_PINGHU,             //平和
    FAN_SIGUIYI,            //四归一
    FAN_SHUANGTONGKE,       //双同刻
    FAN_SHUANGANKE,         //双暗刻
    FAN_ANGANG,             //暗杠
    FAN_DUANYAO,            //断幺
                            //========1番========
    FAN_YIBANGAO,           //一般高
    FAN_XIXIANGFENG,        //喜相逢
    FAN_LIANLIU,            //连六
    FAN_LAOSHAOFU,          //老少副
    FAN_YAOJIUKE,           //幺九刻
    FAN_MINGGANG,           //明杠
    FAN_QUEYIMEN,           //缺一门
    FAN_WUZI,               //无字
    FAN_BIANZHANG,          //边张
    FAN_KANZHANG,           //坎张
    FAN_DANDIAOJIANG,       //单钓将
    FAN_ZIMO,               //自摸
    FAN_HUAPAI,             //花牌
                            //========5番========
    FAN_MINGANGANG,         //明暗杠
    FAN_SIZE                //========番种数量========
};

//番数表
extern const int FAN_SCORE[];
extern const char *FAN_NAME[];

//算番器类
class Fan {
  public:
    std::vector<std::vector<int>> fan_table[FAN_SIZE];          //算番结果：每个番种都是一个vector，用来存放组成该番种的Pack的编号
                                                                //若为特殊和型、整体属性类、和牌方式类，则为-1
    std::vector<std::vector<int>> excluded_fan_table[FAN_SIZE]; //因重复计算需要排除的番种
    std::vector<Pack> fan_packs;                                //最终组合成的pack
    int tot_fan;                                                //最终的番数

    std::vector<std::vector<int>> fan_table_res[FAN_SIZE];
    // std::vector<std::vector<int>> excluded_fan_table_res[FAN_SIZE];
    std::vector<Pack> fan_packs_res;
    int tot_fan_res;

    int JudgeHu(const Handtiles &ht) { //判断是否和牌
        return _JudgeCompleteSpecialHu(ht) || _JudgeQidui(ht) || _JudgeBasicHu(ht) || _JudgeZuhelongBasicHu(ht);
    }
    int JudgeHuTile(Handtiles &ht, Tile &t) { //判断使用t作为第14张牌后是否和牌
        ht.SetTile(t);
        int ret = JudgeHu(ht);
        ht.SetTile(TILE_INVALID);
        return ret;
    }
    std::vector<Tile> CalcTing(const Handtiles &const_ht) { //计算所听的牌
        Handtiles ht = const_ht;
        std::vector<Tile> ting;
        for (int i = 1; i < TILE_SIZE; i++) {
            ht.SetTile(i);
            // if (JudgeHu(ht)) {
            if (JudgeHu(ht) && ht.HandTileCount(i) != 5) { //去掉加上这张牌后立牌中有5张的情况
                ting.push_back(i);
            }
        }
        ht.SetTile(TILE_INVALID);
        return ting;
    }
    void CountFan(const Handtiles &ht) {
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
    /*
     * 番种分类：
     * 完全特殊和型（十三幺、全不靠、七星不靠）可复合：和牌方式类
     * 特殊和型（七对、连七对）可复合：整体属性类、和牌方式类
     * 基本和型、组合龙基本和型：整体属性类、刻杠类、关联组合类、单面子属性类、和牌方式类
     * 
     * 特殊情况：全不靠、七星不靠可复合组合龙；七对需判断是否连七对；无番和；七对和型也有可能组成基本和型；花牌
     */
  private:
#ifdef DEBUG_DFS_CNT
    int _cnt_dfs, _cnt_dfs_res;
    std::chrono::time_point<std::chrono::high_resolution_clock> _time_count_fan_s, _time_count_fan_e, _time_dfs_s, _time_dfs_e;
#endif
    //在 fan_table 中增加番并记录其对应的pack组合
    void _AddFan(fan_t f, const std::vector<int> &v) { fan_table[f].push_back(v); }
    //在 excluded_fan_table 中增加番并记录其对应的pack组合（表示需要排除的番）
    void _ExcludeFan(fan_t f, const std::vector<int> &v) { excluded_fan_table[f].push_back(v); }
    //判断 fan_table 中是否存在某个番种
    int _HasFan(fan_t f) { return !!fan_table[f].size(); }
    //判断 excluded_fan_table 中是否存在某个番种（表示是否需要排除某个番）
    int _HasExcludedFan(fan_t f) { return !!excluded_fan_table[f].size(); }

    //基本和型算番。如果给最后一个参数，表示牌型存在组合龙（但不存在packs中）
    void _CountBasicFan(const Handtiles &ht, const std::vector<Pack> &packs, const Pack &zuhelong_pack) {
        _CountOverallAttrFan(ht, packs, zuhelong_pack);
        _CountKeGangFan(ht, packs);
        _CountAssociatedCombinationFan(ht, packs);
        _CountSinglePackFan(ht, packs);
        _CountWinModeFan(ht, packs, zuhelong_pack, CalcTing(ht));
    }
    //取当前与之前其他牌型组合的计番结果中番数较大的保存，并清空当前计番结果
    void _GetMaxFan() {
        _FanTableExclude();
        _FanTableCount();
        if (tot_fan > tot_fan_res) {
            tot_fan_res = tot_fan;
            for (int i = 1; i < FAN_SIZE; i++) {
                fan_table_res[i] = fan_table[i];
            }
            fan_packs_res = fan_packs;
        }
        _ClearTable();
    }
    //在fan_table中去掉所有存在于excluded_fan_table的番种（pack也相同才算相同）
    void _FanTableExclude() {
        //先找到都有的番，再去除pack组合相同的番
        for (int i = 1; i < FAN_SIZE; i++) {
            if (fan_table[i].size() && excluded_fan_table[i].size()) {
                std::vector<std::vector<int>> v;
                std::vector<int> vis(excluded_fan_table[i].size(), 0);
                std::vector<int> res(fan_table[i].size(), 0);
                for (size_t j = 0; j < excluded_fan_table[i].size(); j++) {
                    if (!vis[j]) {
                        for (size_t k = 0; k < fan_table[i].size(); k++) {
                            if (!res[k] && fan_table[i][k] == excluded_fan_table[i][j]) {
                                res[k] = 1;
                                break;
                            }
                        }
                        vis[j] = 1;
                    }
                }
                for (size_t k = 0; k < fan_table[i].size(); k++) {
                    if (res[k] == 0) {
                        v.push_back(fan_table[i][k]);
                    }
                }
                fan_table[i] = v;
            }
        }
    }
    //通过番种表计算番数，并考虑无番和的情况
    void _FanTableCount() {
        int cnt = 0;
        for (int i = 1; i < FAN_SIZE; i++) {
            cnt += fan_table[i].size() * FAN_SCORE[i];
        }
        if (cnt == 0) { //无番和
            _AddFan(FAN_WUFANHU, {});
            cnt = FAN_SCORE[FAN_WUFANHU];
        }
        tot_fan = cnt;
    }
    //清空番种表：fan_table, excluded_fan_table
    void _ClearTable() {
        for (int i = 1; i < FAN_SIZE; i++) {
            fan_table[i].clear();
            excluded_fan_table[i].clear();
        }
    }
    //清空番数结果
    void _ClearResult() {
        tot_fan_res = 0;
    }
    //清空所有结果：番种表与番数
    void _Clear() {
        _ClearTable();
        _ClearResult();
    }

    /*
     * 整体属性类算番：
     * 绿一色、九莲宝灯、清幺九、字一色、混幺九、全双刻、清一色、全大、全中、全小、全带五、大于五、小于五、推不倒、碰碰和、混一色、五门齐*、全带幺、平和*、四归一*（可以多个）、断幺、缺一门、无字*
     * 其中，九莲宝灯、全双刻、全带五、碰碰和、全带幺、平和不接受七对，但仍然归属于整体属性类，不会影响结果
     * 组合龙可复合：五门齐、平和、四归一、无字
     * _ExcludeYaojiuke 用于清幺九、字一色、混幺九减计幺九刻
     */
    void _ExcludeYaojiuke(const std::vector<Pack> &packs);
    void _CountOverallAttrFan(const Handtiles &ht, const std::vector<Pack> &packs, const Pack &zuhelong_pack);
    /*
     * 刻杠类算番：
     * 四杠、四暗刻、三杠、三暗刻、双暗杠、双明杠、双暗刻、暗杠、明杠、明暗杠
     * 分成可组合的和只能一个的分别计番
     */
    void _CountKeGangFan(const Handtiles &ht, const std::vector<Pack> &packs);
    /*
     * 关联组合类算番：
     * 大四喜、大三元、小四喜、小三元、一色双龙会、一色四同顺、一色四节高、一色四步高、一色三同顺、一色三节高、清龙、三色双龙会、一色三步高、三同刻、三风刻、花龙、三色三同顺、三色三节高、三色三步高、双箭刻、双同刻、一般高、喜相逢、连六、老少副
     * 先统计所有能组成的番种，再进行判断能够不重复组成的番种
     * 使用 _StoreFan 储存能组成的番种
     */
    void _StoreFan(std::vector<std::pair<fan_t, std::vector<int>>> &e, fan_t f, const std::vector<int> &v);
    void _CountAssociatedCombinationFan(const Handtiles &ht, const std::vector<Pack> &packs);
    /*
     * 单面子属性类算番：
     * 箭刻、圈风刻、门风刻、幺九刻
     */
    void _CountSinglePackFan(const Handtiles &ht, const std::vector<Pack> &packs);
    /*
     * 和牌方式类算番：
     * 妙手回春、海底捞月、杠上开花、抢杠和、全求人、不求人、和绝张、门前清、边张、坎张、单钓将、自摸
     */
    void _CountWinModeFan(const Handtiles &ht, const std::vector<Pack> &packs, const Pack &zuhelong_pack, const std::vector<Tile> &ting);

    //判断是否可以组成完全特殊和型（十三幺、全不靠、七星不靠），返回番种编号
    fan_t _JudgeCompleteSpecialHu(const Handtiles &ht);
    //判断是否可以组成七对和型（七对、连七对），返回番种编号
    fan_t _JudgeQidui(const Handtiles &ht);
    //判断是否可以组成基本和型（不包括组合龙）
    int _JudgeBasicHu(const Handtiles &ht);
    //判断是否可以组成组合龙基本和型
    int _JudgeZuhelongBasicHu(const Handtiles &ht);

    //DFS接口
    int _Dfs(const Handtiles &ht, const std::vector<Tile> &sorted_lipai, int mianzi_cnt, int duizi_cnt, std::vector<Pack> &packs, int flag_count_fan, const Pack &zuhelong_pack = Pack());
    //DFS主体：搜索立牌是否可以组成相应数量的面子和对子。flag_count_fan决定是否搜索所有牌型并算番
    int _Dfs_recursive(const Handtiles &ht, const std::vector<Tile> &sorted_lipai, int mianzi_cnt, int duizi_cnt, std::vector<int> &vis, std::vector<Pack> &packs, int flag_count_fan, const Pack &zuhelong_pack, std::unordered_set<long long> &st);
    //生成组成的Packs的Hashcode
    long long _PacksHashcode(const Handtiles &ht, const std::vector<Pack> &packs);
    //判断两张牌是否相同或相邻（在a<b的情况下）

    int _Judge2SameOrAdjacent(const Tile &a, const Tile &b) const;
    //判断三张牌是否可以组成Pack（在都是数或都是字的情况下）
    int _Judge3MakePack(const Tile &a, const Tile &b, const Tile &c) const;
    //判断两张牌是否可以组成Pack（在都是数或都是字的情况下）
    int _Judge2MakePack(const Tile &a, const Tile &b) const;

    //判断bitmap中是否存在组合龙，若有则返回组合龙的type编号，否则返回0
    int _JudgeZuhelong(long long bitmap) const;
    //判断bitmap中的数牌是否为某个组合龙的部分（即数牌是否为全不靠）
    int _JudgePartOfZuhelong(long long bitmap) const;

    //二进制中1的个数
    int _BitCount(long long n) const;
};

} // namespace mahjong

#endif
