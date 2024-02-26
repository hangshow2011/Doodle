//
// Created by TD on 2024/2/26.
//

#include "http_distributed_tasks.h"

#include <doodle_lib/core/http/http_listener.h>
#include <doodle_lib/core/http/http_route.h>
namespace doodle::launch {

bool http_distributed_tasks::operator()(const argh::parser &in_arh, std::vector<std::shared_ptr<void>> &in_vector) {
  default_logger_raw()->log(log_loc(), level::warn, "开始服务器");
  auto l_rout_ptr = std::make_shared<http::http_route>();
  auto l_listener = std::make_shared<http::http_listener>(g_io_context(), l_rout_ptr);
  l_listener->run();
}
}  // namespace doodle::launch