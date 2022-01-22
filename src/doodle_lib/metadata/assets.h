//
// Created by teXiao on 2021/4/27.
//

#pragma once

#include <doodle_lib/doodle_lib_fwd.h>

namespace doodle {
class DOODLELIB_API assets {
  FSys::path p_path;

  void set_path_component();

  std::vector<string> p_component;

 public:
  std::string p_name_show_str;
  assets();
  explicit assets(FSys::path in_name);
  // ~Assets();

  [[nodiscard]] std::string str() const;

  const std::vector<string>& get_path_component() {
    return p_component;
  };

  void set_path(const FSys::path& in_path);
  const FSys::path& get_path() const;

  [[nodiscard]] std::string show_str() const;

  bool operator<(const assets& in_rhs) const;
  bool operator>(const assets& in_rhs) const;
  bool operator<=(const assets& in_rhs) const;
  bool operator>=(const assets& in_rhs) const;
  bool operator==(const assets& in_rhs) const;
  bool operator!=(const assets& in_rhs) const;
  void attribute_widget(const attribute_factory_ptr& in_factoryPtr);

 private:


  friend void to_json(nlohmann::json& j, const assets& p) {
    j["path"]          = p.p_path;
    j["name_show_str"] = p.p_name_show_str;
  }
  friend void from_json(const nlohmann::json& j, assets& p) {
    j.at("path").get_to(p.p_path);
    j.at("name_show_str").get_to(p.p_name_show_str);
    p.set_path_component();
  }
};
}  // namespace doodle


