//
// Created by TD on 2022/8/4.
//

#include "rules.h"

#include <doodle_core/metadata/time_point_wrap.h>
#include <doodle_core/metadata/detail/time_point_info.h>

namespace doodle {
namespace business {

class rules::impl {
 public:
  using time_point_info = ::doodle::business::rules_ns::time_point_info;

  work_day_type work_weekdays{};
  std::vector<std::pair<
      chrono::seconds,
      chrono::seconds>>
      work_pair{};
  std::vector<std::pair<chrono::seconds, chrono::seconds>> extra_holidays{};
  std::vector<time_point_info> extra_work{};
  std::vector<time_point_info> extra_rest{};

  /// \brief 周六 ->周日(index 6->0)
  constexpr static work_day_type work_Monday_to_Friday{0b0111110};
};

void to_json(nlohmann::json& j, const rules& p) {
  j["work_weekdays"]  = p.p_i->work_weekdays;
  j["work_pair"]      = p.p_i->work_pair;
  j["extra_holidays"] = p.p_i->extra_holidays;
  j["extra_work"]     = p.p_i->extra_work;
  j["extra_rest"]     = p.p_i->extra_rest;
}
void from_json(const nlohmann::json& j, rules& p) {
  j.at("work_weekdays").get_to(p.p_i->work_weekdays);
  j.at("work_pair").get_to(p.p_i->work_pair);
  j.at("extra_holidays").get_to(p.p_i->extra_holidays);
  j.at("extra_work").get_to(p.p_i->extra_work);
  j.at("extra_rest").get_to(p.p_i->extra_rest);
}

using namespace rules_ns;
rules::rules()
    : p_i(std::make_unique<impl>()) {
}
void rules::work_weekdays(const rules::work_day_type& in_work_weekdays) {
}
const rules::work_day_type& rules::work_weekdays() const {
  return p_i->work_weekdays;
}
void rules::add_work_time(const chrono::seconds& in_begin, const chrono::seconds& in_end) {
  p_i->work_pair.emplace_back(in_begin, in_end);
}
const std::vector<std::pair<chrono::seconds, chrono::seconds>>& rules::work_time() const {
  return p_i->work_pair;
}
void rules::add_extra_work(const time_point_wrap& in_begin, const time_point_wrap& in_end, const std::string& in_info) {
  p_i->extra_work.emplace_back(in_begin, in_end, in_info);
}
const std::vector<rules_ns::time_point_info>& rules::extra_work() const {
  return p_i->extra_work;
}
void rules::add_extra_rest(const time_point_wrap& in_begin, const time_point_wrap& in_end, const std::string& in_info) {
  p_i->extra_rest.emplace_back(in_begin, in_end, in_info);
}
const std::vector<rules_ns::time_point_info>& rules::extra_rest() const {
  return p_i->extra_rest;
}
rules rules::get_default() {
  rules l_rules{};
  l_rules.p_i->work_weekdays = impl::work_Monday_to_Friday;
  l_rules.p_i->extra_holidays.emplace_back(9h, 12h);
  l_rules.p_i->extra_holidays.emplace_back(13h, 18h);
  return l_rules;
}
rules::rules(rules&& in_rules) noexcept
    : p_i(std::move(in_rules.p_i)) {
}
rules& rules::operator=(rules&& in_rules) noexcept {
  p_i = std::move(in_rules.p_i);
  return *this;
}
std::string rules::debug_print() {
  return fmt::format(
      "规则 周六日规则 {}\n每日规则 {} \n节假日规则 {} \n调休规则 {} \n加班规则 {}",
      p_i->work_weekdays,
      fmt::join(p_i->work_pair, "->"),
      fmt::join(p_i->extra_holidays, "->"),
      fmt::join(p_i->extra_rest, "->"),
      fmt::join(p_i->extra_work, "->"));
}
void rules::add_extra_holidays(const duration_type& in_begin, const duration_type& in_end) {
  p_i->extra_holidays.emplace_back(in_begin, in_end);
}
const std::vector<std::pair<chrono::seconds, chrono::seconds>>& rules::extra_holidays() const {
  return p_i->extra_holidays;
}

rules::~rules() = default;

}  // namespace business
}  // namespace doodle
