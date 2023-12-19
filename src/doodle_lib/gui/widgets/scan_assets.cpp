//
// Created by TD on 2023/12/19.
//

#include "scan_assets.h"

#include <doodle_core/metadata/assets_file.h>
#include <doodle_core/metadata/season.h>

#include <doodle_app/lib_warp/imgui_warp.h>
namespace doodle::gui {

namespace details {

class scan_assets_config {
 public:
  struct project_root_t {
    FSys::path path_;
    std::string name_;
  };
  // 扫瞄资产的根目录
  std::vector<project_root_t> project_roots_;
  // 扫瞄分类

  struct scan_category_t {
    virtual std::vector<entt::handle> scan(const project_root_t& in_root) const                                    = 0;
    virtual std::vector<entt::handle> check_path(const project_root_t& in_root, const entt::handle& in_path) const = 0;
  };
  std::vector<scan_category_t> scan_categorys_;
  // 独步消遥            {"//192.168.10.250/public/DuBuXiaoYao_3", "独步逍遥" },
  // 人间最得意           {"//192.168.10.240/public/renjianzuideyi", "人间最得意" },
  // 无尽神域            {"//192.168.10.240/public/WuJinShenYu", "无尽神域" },
  // 无敌剑魂            {"//192.168.10.240/public/WuDiJianHun", "无敌剑魂" },
  // 万古神话            {"//192.168.10.240/public/WanGuShenHua", "万古神话" },
  // 炼气十万年          {"//192.168.10.240/public/LianQiShiWanNian", "炼气十万年" },
  // 独步万古            {"//192.168.10.240/public/WGXD", "万古邪帝" },
  // 我埋葬了诸天神魔     {"//192.168.10.240/public/LongMaiWuShen", "龙脉武神" },
  // 万域封神           {"//192.168.10.218/WanYuFengShen", "万域封神" }

  scan_assets_config()
      : project_roots_{{"//192.168.10.250/public/DuBuXiaoYao_3", "独步逍遥"},
                       {"//192.168.10.240/public/renjianzuideyi", "人间最得意"},
                       {"//192.168.10.240/public/WuJinShenYu", "无尽神域"},
                       {"//192.168.10.240/public/WuDiJianHun", "无敌剑魂"},
                       {"//192.168.10.240/public/WanGuShenHua", "万古神话"},
                       {"//192.168.10.240/public/LianQiShiWanNian", "炼气十万年"},
                       {"//192.168.10.240/public/WGXD", "万古邪帝"},
                       {"//192.168.10.240/public/LongMaiWuShen", "龙脉武神"},
                       {"//192.168.10.218/WanYuFengShen", "万域封神"}},
        scan_categorys_{} {}

  /// 路径规范
  /// `项目根目录/6-moxing/BG/JD(季数)_(集数开始)/BG(编号)/(场景名称)/Content/(场景名称)/Map/(场景名称)_(版本).umap`
  struct scene_scan_category_t : scan_category_t {
    std::vector<entt::handle> scan(const project_root_t& in_root) const override {
      const FSys::path l_scene_path = in_root.path_ / "6-moxing/BG";
      const std::regex l_JD_regex{R"(JD(\d+)_\d+)"};
      const std::regex l_BG_regex{R"(BG(\d+[a-zA-Z]\d*))"};

      if (!FSys::exists(l_scene_path)) {
        return std::vector<entt::handle>();
      }
      std::vector<entt::handle> l_out;
      std::smatch l_match{};

      for (const auto& l_s : FSys::directory_iterator{l_scene_path}) {  // 迭代一级目录
        auto l_name_str = l_s.path().filename().generic_string();
        if (l_s.is_directory() && std::regex_match(l_name_str, l_match, l_JD_regex)) {  // 检查一级目录
          season l_season{std::stoi(l_match[1].str())};
          for (const auto& l_s2 : FSys::directory_iterator{l_s.path()}) {  // 迭代二级目录
            auto l_name2_str = l_s2.path().filename().generic_string();
            if (l_s2.is_directory() && std::regex_match(l_name2_str, l_match, l_BG_regex)) {  // 检查二级目录
              for (auto&& l_s3 : FSys::directory_iterator{l_s2.path()}) {                     // 迭代三级目录
                if (l_s3.is_directory()) {
                  auto l_dis_path = l_s3.path() / "Content" / l_s3.path().filename() / "Map";  // 确认目标路径
                  if (!FSys::exists(l_dis_path)) continue;
                  for (auto&& l_s4 : FSys::directory_iterator{l_dis_path}) {             // 迭代四级目录
                    if (l_s4.is_regular_file() && l_s4.path().extension() == ".umap") {  // 确认后缀名称
                      auto l_stem = l_s4.path().stem().generic_string();

                      if (l_stem.starts_with(l_s3.path().filename().generic_string()) &&  // 检查文件名和文件格式
                          std::count(l_stem.begin(), l_stem.end(), '_') == 1) {
                        assets_file l_assets_file{l_s3.path(), l_s3.path().filename().generic_string(), 0};

                        auto l_handle = entt::handle{*g_reg(), g_reg()->create()};
                        l_handle.emplace<season>(l_season);
                        l_handle.emplace<assets_file>(std::move(l_assets_file));
                        l_out.push_back(l_handle);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      return l_out;
    }
    /// 检查对于的rig文件和maya文件, maya文件可以不存在, 但是rig文件必须存在
    ///
    /// maya文件(同时也是rig文件):
    ///    项目根目录/6-moxing/BG/JD(季数)_(集数开始)/BG(编号)/Mod/(场景名称)_(版本)_Low.ma

    std::vector<entt::handle> check_path(const project_root_t& in_root, const entt::handle& in_path) const override {
      const FSys::path l_scene_path = in_root.path_ / "6-moxing/BG";

      if (!in_path.any_of<season, assets_file>()) return {};
      if (!FSys::exists(l_scene_path)) return {};

      auto& l_season = in_path.get<season>();
      auto& l_assets = in_path.get<assets_file>();

      // 检查rig文件
    }
  };

  void start_sacn() {}

  // 检查路径
  void check_path() {}
};

}  // namespace details

void scan_assets_t::start_scan() {}

bool scan_assets_t::render() {
  if (ImGui::Button(*start_scan_id)) {
    start_scan();
  }
  return is_open;
}
}  // namespace doodle::gui