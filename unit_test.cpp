#include "console.h"
#include "fan.h"
#include "print.h"
#include "tile.h"

#include <vector>
using std::string;
using std::vector;
using namespace mahjong;

// #define DEBUG_DFS
// #define DEBUG_ASSCOM_TABLE
// #define DEBUG_DFS_CNT

struct Test {
    int cnt;
    int flag_first_test_case;
    Handtiles ht;
    Test() : cnt(0), flag_first_test_case(1) {}
    void operator()(int ret, string s, string correct_s = "", int flag_check = 1) {
        if (!flag_first_test_case) {
            putchar('\n');
        }
        flag_first_test_case = 0;
        cnt++;
        printf("Test case %d: %s, ret = %d\n", cnt, s.c_str(), ret);
        int r = ht.StringToHandtiles(s);
        if (r == ret) {
            printf(" - ✅  Passed: [string check], Handtiles: ");
            if (ret == 0) {
                // printf("\n%7s", "");
                StdPrintHandtiles(ht);
            } else {
                printf("[No handtiles due to invalid string]\n");
            }
        } else {
            printf(" - ❌  Failed: [string check], get %d rather than %d\n", r, ret);
            exit(0);
        }
        if (ret == 0) {
            string outs = ht.HandtilesToString();
            if (correct_s == "") {
                correct_s = s;
            }
            if (flag_check) {
                if (outs == correct_s || outs == correct_s + "|EE0000|" || outs == correct_s + "|") {
                    printf(" - ✅  Passed: [handtiles to string check], standard string = %s\n", outs.c_str());
                } else {
                    printf(" - ❌  Failed: [handtiles to string check], get %s rather than %s\n", outs.c_str(), correct_s.c_str());
                    exit(0);
                }
            } else {
                printf(" - ✅  standard string = %s\n", outs.c_str());
            }
        }
    }
    void operator()(string s, vector<Tile> v) {
        operator()(0, s, "");
        Fan fan;
        sort(v.begin(), v.end());
        vector<Tile> ret = fan.CalcTing(ht);
        if (v == ret) {
            printf(" - ✅  Passed: [ting check], Ting tiles is ");
            for (auto t : v) {
                StdPrintTile(t);
            }
            printf("\n");
        } else {
            printf(" - ❌  Failed: [ting check], Ting tiles is ");
            for (auto t : v) {
                StdPrintTile(t);
            }
            printf(", but get ");
            for (auto t : ret) {
                StdPrintTile(t);
            }
            printf("\n");
            exit(0);
        }
    }
    void operator()(string s, vector<fan_t> v, vector<fan_t> v2 = {}) {
        operator()(0, s, "", false);
        Fan fan;
        sort(v.begin(), v.end());
        fan.CountFan(ht);
        vector<fan_t> res;
        for (int i = 1; i < FAN_SIZE; i++) {
            for (size_t j = 0; j < fan.fan_table_res[i].size(); j++) {
                res.push_back((fan_t)i);
            }
        }
        int failed = 0;
        if (res == v || res == v2) {
            printf(" - ✅  Passed: [fan check], count of fan is %d and fan is:\n", fan.tot_fan_res);
        } else {
            failed = 1;
            printf(" - ❌  Failed: [fan check], expect");
            for (auto i : v) {
                printf(" %s", FAN_NAME[i]);
            }
            printf(" but get %d fan:\n", fan.tot_fan_res);
        }
        for (int i = 1; i < FAN_SIZE; i++) {
            if (i == FAN_HUAPAI) { //花牌单独输出
                continue;
            }
            for (size_t j = 0; j < fan.fan_table_res[i].size(); j++) {
                //常用汉字的UTF-8是3个字节，为了对齐，多一个字就要少输出一个空格
                printf("%*s %2d番", 15 + (int)strlen(FAN_NAME[i]) / 3, FAN_NAME[i], FAN_SCORE[i]);
                string pack_string;
                int flag_first = 1;
                for (auto pid : fan.fan_table_res[i][j]) {
                    if (!flag_first) {
                        pack_string += ',';
                    }
                    flag_first = 0;
                    pack_string += ' ';
                    pack_string += PackToEmojiString(fan.fan_packs_res[pid]);
                }
                printf("%s\n", pack_string.c_str());
            }
        }
        int i = FAN_HUAPAI;
        int cnt_huapai = fan.fan_table_res[i].size();
        if (cnt_huapai) {
            printf("%*s %2d番 × %d\n", 15 + (int)strlen(FAN_NAME[i]) / 3, FAN_NAME[i], FAN_SCORE[i], cnt_huapai);
        }
        if (failed) {
            exit(0);
        }
    }
};

void UnitTest() {
    Test test;
    test(0, "[CCCC][FFFF][PPPP][NNNN]EE", "[CCCC][FFFF][PPPP][NNNN]EE");
    test(0, "[345s,2]34555567p[789m]", "[345s,2][789m,1]3455556p7p");
    test(0, "345s3555567p[789m]4p", "[789m,1]345s3555567p4p");
    test(0, "345s3p55p5567p[789m]4p", "[789m,1]345s3555567p4p");
    test(0, "345s3pFF5567p[789m]4p", "[789m,1]345s35567pFF4p");
    test(0, " 345 s3pFF5 567p[789m]4p ", "[789m,1]345s35567pFF4p");
    test(0, "345s345m55567p[789m]|NE1010", "[789m,1]345m345s5556p7p|NE1010");
    test(0, "345s345m55567p[789m]|NE1010|5", "[789m,1]345m345s5556p7p|NE1010|abcde");
    test(0, "345s345m55567p[789m]|NE1010|cbaghd", "[789m,1]345m345s5556p7p|NE1010|cbaghd");
    test(0, "345s[777m,3]345m55567p|NE1010|cbaghd", "[777m,3]345m345s5556p7p|NE1010|cbaghd");
    test(0, "345s[777m,3]345m55567p|NE1010|3", "[777m,3]345m345s5556p7p|NE1010|abc");
    test(0, "345s[777m,3]345m55567p|NE1010|0", "[777m,3]345m345s5556p7p|NE1010|");
    test(0, "345s[777m,3]345m55567p|NE1010|8", "[777m,3]345m345s5556p7p|NE1010|abcdefgh");
    test(0, "345s3p55p5567p[7777s,1]4p", "[7777s,1]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s,2]4p", "[7777s,2]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s,3]4p", "[7777s,3]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s,5]4p", "[7777s,5]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s,6]4p", "[7777s,6]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s,7]4p", "[7777s,7]345s3555567p4p|EE0000|");
    test(0, "345s3p55p5567p[7777s]4p|EE0100|cbaghdfe", "[7777s]345s3555567p4p|EE0100|cbaghdfe");
    test(0, "345s3pFF5567p[3333m,6]", "[3333m,6]345s35567pFF "); //只有13张的牌
    test(0, "345s3pFF5567pC[3333m,6]", "[3333m,6]345s35567pFFC");
    test(0, "a5sghWSW[3333m,6]78m2s5s|EE0000|dc", "[3333m,6]78m25sSWWagh5s|EE0000|dc"); //手牌中有花牌
    test(-1, "345s35[55567p[789m]4p");
    test(-1, "34Fp5s35[55567p[789m]4p");
    test(-1, "345s345m55567p[789m]|N1011|cbaghd");
    test(-1, "345s345m55567p[789m]||cbaghd");
    test(-1, "345s345m55567p[789m]|cbaghd");
    test(-1, "345s345m55567p[789m]|NE1011|9");
    test(-1, "345s345m55567p[789m]|NE1011|cbagjhd");
    test(-1, "[345sF,2]34555567p[789m]");
    test(-1, "[345,2]34555567p[789m]");
    test(-1, "[3C45,2]34555567p[789m]");
    test(-1, "[345s,2]34555[]567p[789m]");
    test(-1, "345s3pFF5567p[789m,F]4p");
    test(-1, "345s3pFF5567pC[346mC]");
    test(-2, "345s3pFF5567pC[333m,6]");
    test(-2, "345s3pFF5567pC[345m,5]");
    test(-3, "345s3pFF5567pC[346m]");
    test(-4, "345s3pFF5567pC[3456m]");
    test(-5, "345s3pFFP5567pC[2222m]");
    test(-5, "345s3pFFP7pC[2222m]");
    test(-6, "24s2m3pFFP5567p[2222m]");
    test(-6, "a5sghWSW[3333m,6]78m2s5s|EE0000|dca"); //花牌超过一张
    test(-7, "[234m,1][555m,1]567m55576p|EE1001|3"); //杠状态下，杠上开花但手里无杠
    test(-7, "[234m,1][555m,1]567m55566p|EE0001|3"); //杠状态下，抢杠和但手里有该牌
    test(-7, "[234m,1][555m,1]567m55576p|EE0011|3"); //杠状态下，抢杠和但为海底状态
    test(-7, "[234m,1][555m,1]567m55566p|EE0100|3"); //绝张状态，但手里有该牌

    test("[CCCC][FFFF][PPPP][NNNN]E ", vector<Tile>{TILE_E});
    test("19m19s19pESWNCFP ", vector<Tile>{TILE_1m, TILE_1p, TILE_1s, TILE_9m, TILE_9p, TILE_9s, TILE_E, TILE_S, TILE_W, TILE_N, TILE_C, TILE_F, TILE_P});
    test("19m19s19pESWNNFP ", vector<Tile>{TILE_C});
    test("22559m11sEESSPP ", vector<Tile>{TILE_9m});
    test("47m28s369pESWCFP ", vector<Tile>{TILE_1m, TILE_5s, TILE_N});
    test("28m47s369pESWCFP ", vector<Tile>{TILE_1s, TILE_5m, TILE_N});
    test("28m47s369pESWCCP ", vector<Tile>{}); //没听
    test("1112345678999s ", vector<Tile>{TILE_1s, TILE_2s, TILE_3s, TILE_4s, TILE_5s, TILE_6s, TILE_7s, TILE_8s, TILE_9s});
    test("[111s,1]2345678999s ", vector<Tile>{TILE_1s, TILE_2s, TILE_4s, TILE_5s, TILE_7s, TILE_8s});
    test("1112345689999s ", vector<Tile>{TILE_7s});
    test("1112346778999s ", vector<Tile>{TILE_5s, TILE_7s, TILE_8s});
    test("3344455566667m ", vector<Tile>{TILE_2m, TILE_3m, TILE_4m, TILE_5m, TILE_7m, TILE_8m});
    test("234m45s88899pEEE ", vector<Tile>{TILE_3s, TILE_6s});
    test("234m35s88899pEEE ", vector<Tile>{TILE_4s});
    test("234m345s8889pEEE ", vector<Tile>{TILE_7p, TILE_9p});
    test("234m345s6669pEEE ", vector<Tile>{TILE_9p});
    test("234m345s3399pEEE ", vector<Tile>{TILE_3p, TILE_9p});
    test("34444556789pPP ", vector<Tile>{TILE_7p, TILE_P});
    test("23344445m888pWW ", vector<Tile>{TILE_1m, TILE_W}); //虽然听4m，但实际已经没有了
    test("234m345s333pEEEE ", vector<Tile>{});               //虽然听E，但实际已经没有了

    //国标麻将规则释义（网络对局版）的例子
    //EE0000 flag 含义: 圈风、门风、自摸、绝张、海底、杠
    test("[EEE,2][SSSS,1]WWWNN55pN|EE1000", vector<fan_t>{FAN_DASIXI, FAN_HUNYISE, FAN_SHUANGANKE, FAN_MINGGANG, FAN_ZIMO});
    test("[EEE,2][SSSS,1]WWWNN555p|SN1000", vector<fan_t>{FAN_XIAOSIXI, FAN_HUNYISE, FAN_PENGPENGHU, FAN_QUANFENGKE, FAN_SHUANGANKE, FAN_MINGGANG, FAN_ZIMO});
    test("[EEE,2][SSS,1]WWW78m55p9m|NN1000", vector<fan_t>{FAN_SANFENGKE, FAN_QUEYIMEN, FAN_ZIMO});
    test("[EEE,2][SSS,1]WWW99m55p9m|NN1000", vector<fan_t>{FAN_SANFENGKE, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_QUEYIMEN, FAN_ZIMO});
    test("[PPP,2][FFF,3]CC66999sC|EE1000", vector<fan_t>{FAN_DASANYUAN, FAN_HUNYISE, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_ZIMO});
    test("[PPP,2][FFF,3]345p666sCC|EE1000", vector<fan_t>{FAN_XIAOSANYUAN, FAN_QUEYIMEN, FAN_DANDIAOJIANG, FAN_ZIMO});
    test("[PPP,2][FFF,3]3335p999s4p|EE1000", vector<fan_t>{FAN_SHUANGJIANKE, FAN_YAOJIUKE, FAN_QUEYIMEN, FAN_ZIMO});
    test("[EEE,2][SSS,1][FFF,3]WWNNW|WW1000", vector<fan_t>{FAN_XIAOSIXI, FAN_ZIYISE, FAN_QUANFENGKE, FAN_MENFENGKE, FAN_JIANKE, FAN_ZIMO});
    test("EESSWWNNPPFFCC|EE1000", vector<fan_t>{FAN_ZIYISE, FAN_QIDUI, FAN_ZIMO});
    test("[111m,2]111999p11991s|EE1000", vector<fan_t>{FAN_QINGYAOJIU, FAN_SANANKE, FAN_SANTONGKE, FAN_ZIMO});
    test("[1111m,2][111p,3]999p11999s|EE1000", vector<fan_t>{FAN_QINGYAOJIU, FAN_SHUANGANKE, FAN_MINGGANG, FAN_ZIMO});
    test("1199m119999p1991s|EE1000", vector<fan_t>{FAN_QINGYAOJIU, FAN_QIDUI, FAN_SIGUIYI, FAN_ZIMO});
    test("[NNN,3][999s,1][CCC,2]999m11p|ES1000", vector<fan_t>{FAN_HUNYAOJIU, FAN_WUMENQI, FAN_SHUANGTONGKE, FAN_JIANKE, FAN_DANDIAOJIANG, FAN_ZIMO});
    test("99m11pSWWNNPPFFS|EE1000", vector<fan_t>{FAN_HUNYAOJIU, FAN_QIDUI, FAN_QUEYIMEN, FAN_ZIMO});
    test("[123p,1][CCC,1]999m79998p|EE1000", vector<fan_t>{FAN_QUANDAIYAO, FAN_JIANKE, FAN_QUEYIMEN, FAN_LAOSHAOFU, FAN_YAOJIUKE, FAN_ZIMO});
    test("44558m22p225566s8m|EE1000", vector<fan_t>{FAN_QIDUI, FAN_DUANYAO, FAN_ZIMO});
    test("[345m,1]22456p222567s", vector<fan_t>{FAN_SANSESANBUGAO, FAN_DUANYAO});
    test("[EEE,2][SSS,1][CCCC]88m456s|ES1000", vector<fan_t>{FAN_QUANFENGKE, FAN_MENFENGKE, FAN_JIANKE, FAN_ANGANG, FAN_QUEYIMEN, FAN_ZIMO});
    test("[WWW,2][NNNN,3][111p,2]99s55m9s|ES1000", vector<fan_t>{FAN_PENGPENGHU, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_MINGGANG, FAN_ZIMO});
    test("[123s,1][333s,2]45678996s|EE1000", vector<fan_t>{FAN_QINGYISE, FAN_SIGUIYI, FAN_LIANLIU, FAN_ZIMO});
    test("11224455668899p|EE1000", vector<fan_t>{FAN_QINGYISE, FAN_QIDUI, FAN_ZIMO});
    test("[CCC,2][789m,1]12345mNN6m|EE1000", vector<fan_t>{FAN_HUNYISE, FAN_QINGLONG, FAN_JIANKE, FAN_ZIMO});
    test("1122559mNNCCCC9m|EE1000", vector<fan_t>{FAN_HUNYISE, FAN_QIDUI, FAN_SIGUIYI, FAN_ZIMO});
    test("12345789p55678m6p|EE1000", vector<fan_t>{FAN_QINGLONG, FAN_BUQIUREN, FAN_PINGHU, FAN_QUEYIMEN});
    test("556699m22334455p", vector<fan_t>{FAN_QIDUI, FAN_QUEYIMEN, FAN_WUZI});
    test("[678m,1][444s,2][FFF,1]234pWW", vector<fan_t>{FAN_WUMENQI, FAN_JIANKE, FAN_DANDIAOJIANG});
    test("7788m44s3344pWWFF|EE1000", vector<fan_t>{FAN_QIDUI, FAN_WUMENQI, FAN_ZIMO});
    test("[FFF,2]147m258p39sWW6s", vector<fan_t>{FAN_ZUHELONG, FAN_WUMENQI, FAN_JIANKE});
    test("[789m,1][999s,2]7899p789s9p|EE1000", vector<fan_t>{FAN_QUANDA, FAN_SANSESANTONGSHUN, FAN_QUANDAIYAO, FAN_SIGUIYI, FAN_YAOJIUKE, FAN_ZIMO});
    test("[444m,2][666m,3]444p44664s", vector<fan_t>{FAN_QUANZHONG, FAN_QUANSHUANGKE, FAN_SANTONGKE});
    test("11223333p33s1122m|EE1000", vector<fan_t>{FAN_QUANXIAO, FAN_QIDUI, FAN_SIGUIYI, FAN_ZIMO});
    test("[666m,2][777m,2][999m,2]78886m|EE0100", vector<fan_t>{FAN_QINGYISE, FAN_DAYUWU, FAN_HUJUEZHANG, FAN_SIGUIYI, FAN_SIGUIYI, FAN_YAOJIUKE});
    test("[123p,3]23334p222444s|EE1000", vector<fan_t>{FAN_XIAOYUWU, FAN_TUIBUDAO, FAN_SHUANGANKE, FAN_SIGUIYI, FAN_ZIMO});
    test("[345m,1]567m45556p345s", vector<fan_t>{FAN_QUANDAIWU, FAN_SANSESANBUGAO, FAN_PINGHU, FAN_XIXIANGFENG});
    test("[444m,2][666m,3]22244s88p4s", vector<fan_t>{FAN_QUANSHUANGKE, FAN_SHUANGTONGKE});
    test("[111s,3][222s,3][444s,2]333s22p", vector<fan_t>{FAN_YISESIJIEGAO, FAN_XIAOYUWU, FAN_QUEYIMEN, FAN_YAOJIUKE, FAN_DANDIAOJIANG});
    test("[666p,2]77888p678sWW7p", vector<fan_t>{FAN_YISESANJIEGAO, FAN_QUEYIMEN});
    test("[444s,2]333m55pWWCCC5p|EE1000", vector<fan_t>{FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_WUMENQI, FAN_PENGPENGHU, FAN_JIANKE, FAN_ZIMO});
    test("[444s,2]333666m55pWW5p", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE});
    test("[1111m][1111p,2]11134445s|EE1000", vector<fan_t>{FAN_SANTONGKE, FAN_MINGANGANG, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_WUZI, FAN_ZIMO});
    test("[234m,1][555m,1]567m55566p", vector<fan_t>{FAN_SHUANGTONGKE, FAN_SIGUIYI, FAN_DUANYAO, FAN_QUEYIMEN, FAN_LIANLIU});
    test("[222m,1][555m,2]8m222555p8m", vector<fan_t>{FAN_PENGPENGHU, FAN_DUANYAO, FAN_SHUANGTONGKE, FAN_SHUANGTONGKE, FAN_QUEYIMEN, FAN_DANDIAOJIANG, FAN_SHUANGANKE}); //原手册少计了双暗刻
    test("[456s,1][456s,1][456s,3]45s55m6s|EE1100", vector<fan_t>{FAN_YISESITONGSHUN, FAN_QUANZHONG, FAN_QUANDAIWU, FAN_HUJUEZHANG, FAN_PINGHU, FAN_QUEYIMEN, FAN_ZIMO});
    test("[234s,1]22333444sFF2s|EE1000", vector<fan_t>{FAN_LVYISE, FAN_YISESITONGSHUN, FAN_ZIMO});
    test("[456s,1][456s,1]456s77mCCC", vector<fan_t>{FAN_YISESANTONGSHUN, FAN_JIANKE, FAN_QUEYIMEN});
    test("[123m,1][345m,1]67789mCC5m|EE1000", vector<fan_t>{FAN_YISESIBUGAO, FAN_HUNYISE, FAN_ZIMO});
    test("[123p,1][234p,1]344556p99s|EE1000", vector<fan_t>{FAN_YISESIBUGAO, FAN_PINGHU, FAN_QUEYIMEN, FAN_DANDIAOJIANG, FAN_ZIMO});
    test("[123m,1][345m,1]567m34sCC5s|EE1000", vector<fan_t>{FAN_YISESANBUGAO, FAN_XIXIANGFENG, FAN_QUEYIMEN, FAN_ZIMO});
    test("[234p,1]12334567pEE5p|EE1000", vector<fan_t>{FAN_YISESANBUGAO, FAN_HUNYISE, FAN_LIANLIU, FAN_ZIMO});
    test("12345566789s22p4s|EE1000", vector<fan_t>{FAN_QINGLONG, FAN_BUQIUREN, FAN_PINGHU, FAN_YIBANGAO, FAN_QUEYIMEN});
    test("123456m456p11789s", vector<fan_t>{FAN_HUALONG, FAN_MENQIANQING, FAN_PINGHU, FAN_XIXIANGFENG}, vector<fan_t>{FAN_HUALONG, FAN_MENQIANQING, FAN_PINGHU, FAN_LIANLIU}); //喜相逢、连六均可以
    test("[234m,1]11234p233442s", vector<fan_t>{FAN_XIAOYUWU, FAN_SANSESANTONGSHUN, FAN_PINGHU, FAN_YIBANGAO});
    test("[345m,1]55567m456p678s|EE1000", vector<fan_t>{FAN_SANSESANBUGAO, FAN_PINGHU, FAN_DUANYAO, FAN_SIGUIYI, FAN_ZIMO});
    test("12355789m123456p", vector<fan_t>{FAN_MENQIANQING, FAN_PINGHU, FAN_LAOSHAOFU, FAN_LIANLIU, FAN_XIXIANGFENG, FAN_QUEYIMEN});
    test("[123m,1][123p,1]123m12p44s3p", vector<fan_t>{FAN_XIAOYUWU, FAN_PINGHU, FAN_YIBANGAO, FAN_YIBANGAO, FAN_XIXIANGFENG, FAN_BIANZHANG}, vector<fan_t>{FAN_XIAOYUWU, FAN_PINGHU, FAN_YIBANGAO, FAN_XIXIANGFENG, FAN_XIXIANGFENG, FAN_BIANZHANG});
    test("234567p34567888s", vector<fan_t>{FAN_MENQIANQING, FAN_PINGHU, FAN_DUANYAO, FAN_LIANLIU, FAN_LIANLIU, FAN_QUEYIMEN});
    test("19m19p119sESWNPFC|EE1000", vector<fan_t>{FAN_SHISANYAO, FAN_ZIMO});
    test("147m28p69sESWNPF3s|EE1000", vector<fan_t>{FAN_QUANBUKAO, FAN_ZIMO});
    test("14m369p25sESWNPFC|EE1000", vector<fan_t>{FAN_QIXINGBUKAO, FAN_ZIMO});
    test("369m147p25sSWNPF8s", vector<fan_t>{FAN_QUANBUKAO, FAN_ZUHELONG});
    test("[234p,1]258m369p14788s", vector<fan_t>{FAN_ZUHELONG, FAN_PINGHU, FAN_DANDIAOJIANG});
    test("1111m99p99sWWNNPP", vector<fan_t>{FAN_HUNYAOJIU, FAN_QIDUI, FAN_WUMENQI, FAN_SIGUIYI});
    test("[CCCC][FFFF]333p67pPP5p|EE1000", vector<fan_t>{FAN_XIAOSANYUAN, FAN_SANANKE, FAN_HUNYISE, FAN_SHUANGANGANG, FAN_BUQIUREN});
    test("[234m,1][345p,1][234s,1][CCCC,2]11m", vector<fan_t>{FAN_QUANQIUREN, FAN_JIANKE, FAN_MINGGANG, FAN_XIXIANGFENG});
    test("[234s,2][666s,2]33888sFFF|EE1000", vector<fan_t>{FAN_LVYISE, FAN_SHUANGANKE, FAN_JIANKE, FAN_ZIMO});
    test("22333344668888s", vector<fan_t>{FAN_LVYISE, FAN_QINGYISE, FAN_QIDUI, FAN_DUANYAO, FAN_SIGUIYI, FAN_SIGUIYI});
    test("11123456789999m|EE1000", vector<fan_t>{FAN_JIULIANBAODENG, FAN_QINGLONG, FAN_SIGUIYI, FAN_ZIMO});
    test("11123456789992p", vector<fan_t>{FAN_JIULIANBAODENG, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_LIANLIU});
    test("22334556677884s", vector<fan_t>{FAN_LIANQIDUI, FAN_DUANYAO});
    test("[123p,1]12355778998p|EE1000", vector<fan_t>{FAN_YISESHUANGLONGHUI, FAN_KANZHANG, FAN_ZIMO});
    test("[123m,1][789m,1][123p,1]89p55s7p|EE1000", vector<fan_t>{FAN_SANSESHUANGLONGHUI, FAN_BIANZHANG, FAN_ZIMO});
    test("[234p,1]123345p88999s", vector<fan_t>{FAN_YISESANBUGAO, FAN_TUIBUDAO, FAN_YAOJIUKE, FAN_WUZI});
    test("223344558pPPPP8p|EE1000", vector<fan_t>{FAN_QIDUI, FAN_TUIBUDAO, FAN_HUNYISE, FAN_SIGUIYI, FAN_ZIMO});
    test("[111m,1][123m,1]33455553m|EE1000", vector<fan_t>{FAN_QINGYISE, FAN_SIGUIYI, FAN_SIGUIYI, FAN_SIGUIYI, FAN_YAOJIUKE, FAN_ZIMO});
    test("22448p55m88sPPPP8p|EE1000", vector<fan_t>{FAN_QIDUI, FAN_SIGUIYI, FAN_ZIMO});
    test("[123m,1][345m,1]678p56sWW7s", vector<fan_t>{FAN_WUFANHU});

    //理论最高番（注意：N不是绝张）
    test("[CCCC][FFFF][PPPP][EEEE]NN|EE1011|8", vector<fan_t>{FAN_DASANYUAN, FAN_SIGANG, FAN_ZIYISE, FAN_SIANKE, FAN_MIAOSHOUHUICHUN, FAN_GANGSHANGKAIHUA, FAN_QUANFENGKE, FAN_MENFENGKE, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI});
    //这个例子的搜索量比较大
    test("23422333444sFF2s|EE1000", vector<fan_t>{FAN_LVYISE, FAN_YISESITONGSHUN, FAN_BUQIUREN});
    test("23422333444562s|EE1000", vector<fan_t>{FAN_QINGYISE, FAN_BUQIUREN, FAN_SIGUIYI, FAN_SIGUIYI, FAN_SIGUIYI, FAN_SHUANGANKE, FAN_DUANYAO});
    //喜相逢、连六
    test("234567p234567sEE", vector<fan_t>{FAN_MENQIANQING, FAN_XIXIANGFENG, FAN_XIXIANGFENG, FAN_LIANLIU, FAN_QUEYIMEN, FAN_DANDIAOJIANG}, vector<fan_t>{FAN_MENQIANQING, FAN_XIXIANGFENG, FAN_LIANLIU, FAN_LIANLIU, FAN_QUEYIMEN, FAN_DANDIAOJIANG});
    //组合龙相关
    test("258m23469p14788s3p", vector<fan_t>{FAN_ZUHELONG, FAN_MENQIANQING, FAN_PINGHU});
    test("258m13369p14788s2p", vector<fan_t>{FAN_ZUHELONG, FAN_MENQIANQING, FAN_PINGHU, FAN_KANZHANG});
    //杠上开花
    test("[1111m][111p,3]999p11999s|EE1001", vector<fan_t>{FAN_QINGYAOJIU, FAN_SANANKE, FAN_GANGSHANGKAIHUA, FAN_ANGANG});
    //抢杠和
    test("111999p11999s798p|EE0001", vector<fan_t>{FAN_SANANKE, FAN_QIANGGANGHU, FAN_QUANDAIYAO, FAN_MENQIANQING, FAN_SIGUIYI, FAN_SHUANGTONGKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_QUEYIMEN, FAN_WUZI, FAN_KANZHANG});
    test("[111p,3]999p11999s798p|EE0001", vector<fan_t>{FAN_QIANGGANGHU, FAN_QUANDAIYAO, FAN_SIGUIYI, FAN_SHUANGTONGKE, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_YAOJIUKE, FAN_QUEYIMEN, FAN_WUZI, FAN_KANZHANG});
    test("[234m,1][555m,1]567m55576p|EE0101|3", vector<fan_t>{FAN_QIANGGANGHU, FAN_SIGUIYI, FAN_DUANYAO, FAN_XIXIANGFENG, FAN_LIANLIU, FAN_QUEYIMEN, FAN_HUAPAI, FAN_HUAPAI, FAN_HUAPAI});
    //妙手回春
    test("[123m,1][345m,1]567m34sCC5s|EE1110", vector<fan_t>{FAN_YISESANBUGAO, FAN_MIAOSHOUHUICHUN, FAN_HUJUEZHANG, FAN_XIXIANGFENG, FAN_QUEYIMEN});
    //海底捞月
    test("[123m,1][345m,1]567m34sCC5s|EE0110", vector<fan_t>{FAN_YISESANBUGAO, FAN_HAIDILAOYUE, FAN_HUJUEZHANG, FAN_XIXIANGFENG, FAN_QUEYIMEN});
    //各种和型加花牌
    test("12345789p55678m6p|EE1000|c", vector<fan_t>{FAN_QINGLONG, FAN_BUQIUREN, FAN_PINGHU, FAN_QUEYIMEN, FAN_HUAPAI});
    test("[123m,1][345m,1]678p56sWW7s|EE0000|2", vector<fan_t>{FAN_WUFANHU, FAN_HUAPAI, FAN_HUAPAI});
    test("258m23469p14788s3p|EE0000|1", vector<fan_t>{FAN_ZUHELONG, FAN_MENQIANQING, FAN_PINGHU, FAN_HUAPAI});
    test("14m369p25sESWNPFC|EE1000|h", vector<fan_t>{FAN_QIXINGBUKAO, FAN_ZIMO, FAN_HUAPAI});
    test("EESSWWNNPPFFCC|EE1000|b", vector<fan_t>{FAN_ZIYISE, FAN_QIDUI, FAN_ZIMO, FAN_HUAPAI});
    test("19m19p119sESWNPFC|EE1000|d", vector<fan_t>{FAN_SHISANYAO, FAN_ZIMO, FAN_HUAPAI});
    //就高不就低原则
    test("55s33344455566m5s", vector<fan_t>{FAN_YISESANJIEGAO, FAN_SANANKE, FAN_PENGPENGHU, FAN_MENQIANQING, FAN_SHUANGTONGKE, FAN_DUANYAO, FAN_QUEYIMEN});
    //不相同原则、不拆移原则、套算一次原则
    test("123789s123789p33m", vector<fan_t>{FAN_MENQIANQING, FAN_PINGHU, FAN_XIXIANGFENG, FAN_XIXIANGFENG, FAN_LAOSHAOFU, FAN_DANDIAOJIANG}, vector<fan_t>{FAN_MENQIANQING, FAN_PINGHU, FAN_XIXIANGFENG, FAN_LAOSHAOFU, FAN_LAOSHAOFU, FAN_DANDIAOJIANG});
    test("789m456p123s123p33m", vector<fan_t>{FAN_HUALONG, FAN_MENQIANQING, FAN_PINGHU, FAN_XIXIANGFENG, FAN_DANDIAOJIANG}, vector<fan_t>{FAN_HUALONG, FAN_MENQIANQING, FAN_PINGHU, FAN_LIANLIU, FAN_DANDIAOJIANG});
    test("12334556778911s", vector<fan_t>{FAN_YISESIBUGAO, FAN_QINGYISE, FAN_MENQIANQING, FAN_PINGHU});
    test("12334556723499m", vector<fan_t>{FAN_QINGYISE, FAN_YISESANBUGAO, FAN_MENQIANQING, FAN_PINGHU, FAN_LIANLIU, FAN_DANDIAOJIANG});
    //非九莲宝灯
    test("11134567889992p", vector<fan_t>{FAN_QINGYISE, FAN_MENQIANQING, FAN_SHUANGANKE, FAN_LIANLIU, FAN_YAOJIUKE, FAN_YAOJIUKE});
    test("[789p,1]11123456994p", vector<fan_t>{FAN_QINGYISE, FAN_LIANLIU, FAN_YAOJIUKE});
    //刻杠类组合
    test("[1111s][2222m][3333p][4444p]88s", vector<fan_t>{FAN_SIGANG, FAN_SIANKE, FAN_SANSESANJIEGAO, FAN_YAOJIUKE, FAN_WUZI});
    test("[1111s][2222m][3333p,7][4444p]88s", vector<fan_t>{FAN_SIGANG, FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_YAOJIUKE, FAN_WUZI});
    test("[1111s][2222m,2][3333p,7][4444p]88s", vector<fan_t>{FAN_SIGANG, FAN_SHUANGANKE, FAN_SANSESANJIEGAO, FAN_YAOJIUKE, FAN_WUZI});
    test("[1111s,1][2222m,2][3333p,7][4444p]88s", vector<fan_t>{FAN_SIGANG, FAN_SANSESANJIEGAO, FAN_YAOJIUKE, FAN_WUZI});
    test("[1111s][2222m][4444p]333p88s", vector<fan_t>{FAN_SANGANG, FAN_SIANKE, FAN_SANSESANJIEGAO, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m][4444p][333p,1]88s", vector<fan_t>{FAN_SANGANG, FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m][4444p,3]333p88s", vector<fan_t>{FAN_SANGANG, FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m][4444p,3][333p,1]88s", vector<fan_t>{FAN_SANGANG, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m,1][4444p,3]333p88s", vector<fan_t>{FAN_SANGANG, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m,1][4444p,3][333p,1]88s", vector<fan_t>{FAN_SANGANG, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m]333444p88s", vector<fan_t>{FAN_SIANKE, FAN_SANSESANJIEGAO, FAN_SHUANGANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m][333p,1]444p88s", vector<fan_t>{FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][2222m,1]333444p88s", vector<fan_t>{FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG, FAN_MINGANGANG});
    test("[1111s][2222m,1][333p,1]444p88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG, FAN_MINGANGANG});
    test("[1111s,1][2222m,1]333444p88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGMINGGANG, FAN_SHUANGANKE, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s]222m333444p88s", vector<fan_t>{FAN_SIANKE, FAN_SANSESANJIEGAO, FAN_ANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][222m,1]333444p88s", vector<fan_t>{FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_ANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][222m,1][333p,1]444p88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_ANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s,1]222m333444p88s", vector<fan_t>{FAN_SANANKE, FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_MINGGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s,1][222m,1]333444p88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANKE, FAN_MINGGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    //刻杠类单独
    test("[1111s][2222m][333p,1][444p,1]88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_SHUANGANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s][222m,1][333p,1][444p,1]88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_ANGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
    test("[1111s,1][222m,1][333p,1]444p88s", vector<fan_t>{FAN_SANSESANJIEGAO, FAN_PENGPENGHU, FAN_MINGGANG, FAN_YAOJIUKE, FAN_WUZI, FAN_DANDIAOJIANG});
}

int main() {
    UnitTest();
}
