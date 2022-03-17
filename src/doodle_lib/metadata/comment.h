﻿//
// Created by TD on 2021/5/18.
//

#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/metadata/leaf_meta.h>
#include <doodle_lib/metadata/tree_adapter.h>

namespace doodle {

class DOODLELIB_API comment {
 public:
  std::string p_comment;
  std::string p_user;

  comment();
  explicit comment(std::string in_str);
  [[nodiscard]] const std::string& get_comment() const;
  void set_comment(const std::string& in_comment);
  [[nodiscard]] const std::string& get_user() const;
  void set_user(const std::string& in_user);
  // DOODLE_MOVE(comment);

 private:
  friend void to_json(nlohmann::json& j, const comment& p) {
    j["comment"] = p.p_comment;
    j["user"]    = p.p_user;
  }
  friend void from_json(const nlohmann::json& j, comment& p) {
    j.at("comment").get_to(p.p_comment);
    j.at("user").get_to(p.p_user);
  }
};

}  // namespace doodle

namespace fmt {
template <>
struct formatter<doodle::comment> : formatter<string_view> {
  template <typename FormatContext>
  auto format(const doodle::comment& in_, FormatContext& ctx) {
    formatter<string_view>::format(
        in_.get_comment(),
        ctx);
  }
};
}  // namespace fmt
