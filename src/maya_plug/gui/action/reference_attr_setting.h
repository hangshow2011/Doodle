//
// Created by TD on 2021/10/14.
//

#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/gui/gui_ref/base_window.h>
namespace doodle::maya_plug {
class reference_file;

namespace reference_attr {
class data {
 public:
  std::string path;
  bool use_sim;
  bool operator==(const data& in_rhs) const;
  bool operator!=(const data& in_rhs) const;
  friend void to_json(nlohmann::json& j, const data& p) {
    j["path"]    = p.path;
    j["use_sim"] = p.use_sim;
  }
  friend void from_json(const nlohmann::json& j, data& p) {
    p.path    = j.at("path");
    p.use_sim = j.at("use_sim");
  }
};
using data_ptr = std::shared_ptr<data>;
}  // namespace reference_attr

/**
 * @brief 引用文件标签命令
 *
 * @li 添加引用文件解算标签
 * @li 在没有引用文件标记时， 使用所有载入的应用进行解算
 *
 */
class reference_attr_setting
    : public gui::window_panel {
  std::vector<entt::handle> p_handle;
  entt::handle p_current_select;
  bool get_file_info();
  void clear();

 public:
  reference_attr_setting();
  ~reference_attr_setting() override;
  constexpr static std::string_view name{"引用编辑"};
  void render() override;
};

namespace reference_attr_setting_ns {
constexpr auto init = []() {
  entt::meta<reference_attr_setting>()
      .type()
      .prop("name"_hs, std::string{reference_attr_setting::name})
      .base<gui::window_panel>();
};
class init_class
    : public init_register::registrar_lambda<init, 3> {};
}  // namespace reference_attr_setting_ns
}  // namespace doodle::maya_plug
