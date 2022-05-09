//
// Created by TD on 2022/5/9.
//

#pragma once

#include <doodle_core/doodle_core_fwd.h>
namespace doodle {
class redirection_path_info;
void to_json(nlohmann::json& j, const redirection_path_info& p);
void from_json(const nlohmann::json& j, redirection_path_info& p);

class DOODLE_CORE_EXPORT redirection_path_info {
 public:
  redirection_path_info();
  explicit redirection_path_info(
      std::string in_token,
      std::vector<FSys::path> in_search_path,
      FSys::path in_file_name);
  std::string token_;
  std::vector<FSys::path> search_path_;
  FSys::path file_name_;

  std::optional<FSys::path> get_replace_path() const;

 private:
  friend void to_json(nlohmann::json& j, const redirection_path_info& p);
  friend void from_json(const nlohmann::json& j, redirection_path_info& p);
};

}  // namespace doodle
