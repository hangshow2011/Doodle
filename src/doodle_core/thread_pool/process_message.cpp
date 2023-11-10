#include "process_message.h"

#include <doodle_core/core/doodle_lib.h>
#include <doodle_core/lib_warp/boost_fmt_rational.h>
#include <doodle_core/lib_warp/json_warp.h>

#include <boost/locale.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <magic_enum.hpp>
#include <range/v3/all.hpp>
#include <range/v3/range.hpp>
#include <spdlog/spdlog.h>

namespace doodle {
void to_json(nlohmann::json& nlohmann_json_j, const process_message& nlohmann_json_t) {
  nlohmann_json_j["time"]     = nlohmann_json_t.p_time;
  nlohmann_json_j["end"]      = nlohmann_json_t.p_end;
  //  nlohmann_json_j["err"]      = nlohmann_json_t.p_err;
  //  nlohmann_json_j["log"]      = nlohmann_json_t.p_log;
  nlohmann_json_j["str_end"]  = nlohmann_json_t.p_str_end;
  nlohmann_json_j["name"]     = nlohmann_json_t.p_name;
  nlohmann_json_j["name_id"]  = nlohmann_json_t.p_name_id;
  nlohmann_json_j["state"]    = nlohmann_json_t.p_state;
  nlohmann_json_j["progress"] = nlohmann_json_t.p_progress;
}
void from_json(const nlohmann::json& nlohmann_json_j, process_message& nlohmann_json_t) {
  nlohmann_json_j.at("time").get_to(nlohmann_json_t.p_time);
  nlohmann_json_j.at("end").get_to(nlohmann_json_t.p_end);
  //  nlohmann_json_j.at("err").get_to(nlohmann_json_t.p_err);
  //  nlohmann_json_j.at("log").get_to(nlohmann_json_t.p_log);
  nlohmann_json_j.at("str_end").get_to(nlohmann_json_t.p_str_end);
  nlohmann_json_j.at("name").get_to(nlohmann_json_t.p_name);
  nlohmann_json_j.at("name_id").get_to(nlohmann_json_t.p_name_id);
  nlohmann_json_j.at("state").get_to(nlohmann_json_t.p_state);
  nlohmann_json_j.at("progress").get_to(nlohmann_json_t.p_progress);
}
process_message::process_message() : p_state(state::wait), p_time(chrono::system_clock::now()), p_name_id("##none") {}

const std::string& process_message::get_name() const {
  //  std::lock_guard _lock{_mutex};
  return p_name;
}
void process_message::set_name(const std::string& in_string) {
  std::lock_guard _lock{_mutex};
  p_name    = in_string;
  p_name_id = fmt::format("{}##{}", get_name(), fmt::ptr(this));
}
void process_message::progress_step(const rational_int& in_rational_int) {
  std::lock_guard _lock{_mutex};
  p_progress += in_rational_int;
  if (p_progress > 1) --p_progress;
}
void process_message::message(const std::string& in_string, const level& in_level_enum) {
  auto l_msg{in_string};

  static boost::locale::generator k_gen{};
  //  k_gen.categories(boost::locale::all_categories ^ boost::locale::formatting_facet ^ boost::locale::parsing_facet);

  static auto l_local{k_gen("zh_CN.UTF-8")};
  if (ranges::all_of(l_msg, [&](const std::string::value_type& in_type) -> bool {
        return std::isspace(in_type, l_local);
      })) {
    return;
  }
  //  l_msg |= ranges::actions::remove_if([](const std::string::value_type& in_type) -> bool {
  //    return std::isspace(in_type, l_local);
  //  });
  spdlog::info(l_msg);
  if (l_msg.back() != '\n') {
    l_msg += '\n';
  }

  std::lock_guard l_lock{_mutex};
  switch (in_level_enum) {
    case level::warning:
      p_err += l_msg;
      p_str_end = l_msg;
      break;
    default:
      p_log += l_msg;
      break;
  }
}
void process_message::set_state(state in_state) {
  std::lock_guard _lock{_mutex};
  switch (in_state) {
    case run:
      p_time = chrono::system_clock::now();
    case wait:
      break;
    case success:
    case fail:
      p_end      = chrono::system_clock::now();
      p_progress = {1, 1};
      break;
  }
  p_state = in_state;
}
std::string_view process_message::err() const {
  //  std::lock_guard _lock{_mutex};
  return p_err;
}
std::string_view process_message::err(std::size_t in_begin, std::size_t in_end) const {
  auto l_it_begin{p_err.begin()};
  auto l_it_end{p_err.end()};
  auto l_b_s{std::min(std::min(in_begin, in_end), p_err.size())};
  auto l_e_s{std::min(std::max(in_begin, in_end), p_err.size())};

  l_it_begin += l_b_s;
  l_it_end += l_e_s;
  std::size_t const l_dis = std::distance(l_it_begin, l_it_end);
  return std::string_view{&(*l_it_begin), l_dis};
}
std::string_view process_message::log() const {
  //  std::lock_guard _lock{_mutex};
  return p_log;
}
std::string_view process_message::log(std::size_t in_begin, std::size_t in_end) const {
  auto l_it_begin{p_log.begin()};
  auto l_it_end{p_log.end()};
  auto l_b_s{std::min(std::min(in_begin, in_end), p_log.size())};
  auto l_e_s{std::min(std::max(in_begin, in_end), p_log.size())};

  l_it_begin += l_b_s;
  l_it_end += l_e_s;
  std::size_t const l_dis = std::distance(l_it_begin, l_it_end);
  return std::string_view{&(*l_it_begin), l_dis};
}
rational_int process_message::get_progress() const {
  //  std::lock_guard _lock{_mutex};
  return p_progress;
}
const process_message::state& process_message::get_state() const {
  //  std::lock_guard _lock{_mutex};
  return p_state;
}
chrono::sys_time_pos::duration process_message::get_time() const {
  //  std::lock_guard _lock{_mutex};

  switch (p_state) {
    case state::wait:
      return chrono::sys_time_pos::duration{0};
    case state::run:
      return chrono::system_clock::now() - p_time;
    case state::fail:
    case state::success:
      return p_end - p_time;
  }
  return {};
}
const std::string& process_message::message_back() const { return p_str_end; }

process_message::process_message(process_message&& in) noexcept {
  std::lock_guard _lock{_mutex};
  //  std::lock_guard _lock_in{in._mutex};
  p_time     = in.p_time;
  p_end      = in.p_end;
  p_err      = std::move(in.p_err);
  p_log      = std::move(in.p_log);
  p_name     = std::move(in.p_name);
  p_state    = in.p_state;
  p_progress = in.p_progress;
}
process_message& process_message::operator=(process_message&& in) noexcept {
  std::lock_guard _lock{_mutex};
  //  std::lock_guard _lock_in{in._mutex};
  p_time     = in.p_time;
  p_end      = in.p_end;
  p_err      = std::move(in.p_err);
  p_log      = std::move(in.p_log);
  p_name     = std::move(in.p_name);
  p_state    = in.p_state;
  p_progress = in.p_progress;
  return *this;
}
process_message::process_message(const process_message& in) noexcept {
  std::lock_guard _lock{_mutex};
  p_time     = in.p_time;
  p_end      = in.p_end;
  p_err      = in.p_err;
  p_log      = in.p_log;
  p_name     = in.p_name;
  p_state    = in.p_state;
  p_progress = in.p_progress;
}
process_message& process_message::operator=(const process_message& in) noexcept {
  std::lock_guard _lock{_mutex};
  p_time     = in.p_time;
  p_end      = in.p_end;
  p_err      = in.p_err;
  p_log      = in.p_log;
  p_name     = in.p_name;
  p_state    = in.p_state;
  p_progress = in.p_progress;
  return *this;
}
const std::string& process_message::get_name_id() const { return p_name_id; }
void process_message::progress_clear() {
  std::lock_guard _lock{_mutex};
  p_progress = 0;
}

}  // namespace doodle
