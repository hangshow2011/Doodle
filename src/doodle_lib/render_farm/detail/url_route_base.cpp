//
// Created by td_main on 2023/8/9.
//

#include "url_route_base.h"

#include <doodle_core/exception/exception.h>
namespace doodle::render_farm::detail {

void http_route::capture_url::set_cap_bit() {
  for (const auto& l_str : capture_vector_) {
    if (l_str.front() == '{' && l_str.back() == '}') {
      capture_bitset_.push_back(true);
    } else {
      capture_bitset_.push_back(false);
    }
  }
}
std::tuple<bool, std::map<std::string, std::string>> http_route::capture_url::match_url(
    boost::urls::segments_ref in_segments_ref
) const {
  bool l_result = false;
  auto l_it     = in_segments_ref.begin();
  std::map<std::string, std::string> l_str{};
  for (auto i = 0; l_it != in_segments_ref.end(); ++l_it, ++i) {
    if (capture_bitset_[i]) {
      l_str.emplace(capture_vector_[i].substr(1, capture_vector_[i].size() - 1), *l_it);
    } else {
      if (capture_vector_[i] != *l_it) {
        break;
      }
    }
  }
  return {l_result, l_str};
}

void http_route::capture_url::operator()(boost::urls::segments_ref in_segments_ref, const entt::handle& in_session) {
  auto [l_result, l_map] = match_url(in_segments_ref);
  if (l_result) {
    action_(in_session, l_map);
  }
}
void http_route::reg(
    boost::beast::http::verb in_verb, std::vector<std::string> in_vector,
    http_route::capture_url::action_type in_function
) {
  actions[in_verb].emplace_back(std::move(in_vector), std::move(in_function));
}
}  // namespace doodle::render_farm::detail
