#pragma once
#include <doodle_core/doodle_core_fwd.h>

namespace doodle {
namespace project_config {
class base_config;

}  // namespace project_config

class organization;

/**
 * 项目信息类
 */
class DOODLE_CORE_EXPORT project {
 public:
  std::string p_name;
  FSys::path p_path;

  std::string p_en_str;
  std::string p_shor_str;

 private:
  void init_name();

 public:
  project();
  explicit project(FSys::path in_path, std::string in_name);

  [[nodiscard]] const std::string& get_name() const;
  void set_name(const std::string& Name) noexcept;

  [[nodiscard]] const FSys::path& get_path() const noexcept;
  [[nodiscard]] FSys::path make_path(const FSys::path& in_path) const;
  void set_path(const FSys::path& Path);

  [[nodiscard]] std::string str() const;
  [[nodiscard]] std::string show_str() const;

  [[nodiscard]] std::string short_str() const;

  static entt::handle get_current();
  static bool has_prj();

  bool operator<(const project& in_rhs) const;
  bool operator>(const project& in_rhs) const;
  bool operator<=(const project& in_rhs) const;
  bool operator>=(const project& in_rhs) const;
  bool operator==(const project& in_rhs) const;
  bool operator!=(const project& in_rhs) const;

 private:
  friend void to_json(nlohmann::json& j, const project& p) {
    j["name"] = p.p_name;
    j["path"] = p.p_path;
  }
  friend void from_json(const nlohmann::json& j, project& p) {
    j.at("name").get_to(p.p_name);
    j.at("path").get_to(p.p_path);
    p.init_name();
  }
};
namespace project_config {
void to_json(nlohmann::json& j, const base_config& p);
void from_json(const nlohmann::json& j, base_config& p);
class DOODLE_CORE_EXPORT base_config {
 private:
  friend void to_json(nlohmann::json& j, const base_config& p);
  friend void from_json(const nlohmann::json& j, base_config& p);

 public:
  constexpr static std::uint32_t class_hash() {
    return "class doodle::project::cloth_config"_hs;
  }

 public:
  FSys::path vfx_cloth_sim_path;

  /**
   * @brief 导出整个解算文件所需要的选择组
   * 我们使用这个组导出整个解算结果
   */
  std::string export_group;
  /**
   * @brief 导出整个解算文件所需要的选择组
   * 我们使用这个组导出整个解算结果
   */
  std::string cloth_proxy_;
  /**
   * @brief 导出整个解算文件所需要的选择组
   * 我们使用这个组导出整个解算结果
   */
  std::string simple_module_proxy_;

  /**
   * @brief 寻找拖入文件时的图标的正则表达式
   */
  std::string find_icon_regex;

  /**
   * @brief 本组的各种分类
   *
   */
  std::vector<std::string> assets_list;

  /**
   * @brief 本组的各种分类
   *
   */
  std::vector<std::string> icon_extensions;

  base_config();

  bool match_icon_extensions(const FSys::path& in_path) const;
};

}  // namespace project_config
}  // namespace doodle
