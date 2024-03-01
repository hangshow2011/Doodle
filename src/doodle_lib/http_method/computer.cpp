//
// Created by TD on 2024/2/26.
//

#include "computer.h"

#include <doodle_core/doodle_core_fwd.h>
#include <doodle_core/lib_warp/boost_fmt_error.h>
#include <doodle_core/metadata/computer.h>
#include <doodle_core/metadata/server_task_info.h>

#include "doodle_lib/core/http/http_session_data.h"
#include "doodle_lib/core/http/http_websocket_data.h"
#include <doodle_lib/core/http/json_body.h>
namespace doodle::http {
void computer::websocket_route(const nlohmann::json &in_json, const entt::handle in_handle) {
  auto l_logger = in_handle.get<socket_logger>().logger_;

  switch (in_json["type"].get<computer_websocket_fun>()) {
    case computer_websocket_fun::set_state: {
      l_logger->log(log_loc(), level::info, "set_state");
      if (!in_json.contains("state") || !in_json["state"].is_string()) break;
      in_handle.get<doodle::computer>().client_status_ =
          magic_enum::enum_cast<doodle::computer_status>(in_json["state"].get<std::string>())
              .value_or(doodle::computer_status::unknown);

      break;
    }
    case computer_websocket_fun::set_task: {
      l_logger->log(log_loc(), level::info, "set_task");
      if (!in_handle.any_of<task_ref>()) break;
      entt::handle l_task_handle = in_handle.get<task_ref>();
      if (!l_task_handle) {
        l_logger->log(log_loc(), level::err, "task_ref is null");
        break;
      }
      if (!l_task_handle.any_of<server_task_info>()) {
        l_logger->log(log_loc(), level::err, "not has server_task_info component");
        break;
      }

      auto &l_task   = l_task_handle.get<server_task_info>();
      l_task.status_ = magic_enum::enum_cast<server_task_info_status>(in_json["status"].get<std::string>())
                           .value_or(server_task_info_status::unknown);
      if (l_task.status_ == server_task_info_status::completed || l_task.status_ == server_task_info_status::failed) {
        l_task.end_time_ = std::chrono::system_clock::now();
      }
      break;
    }
    case computer_websocket_fun::logger: {
      l_logger->log(log_loc(), level::info, "logger");
      //      if (!in_json.contains("level") || !l_json.contains("msg")) break;
      //      break;
    }
  };
}
void computer::list_computers(boost::system::error_code in_error_code, const entt::handle in_handle) {
  std::vector<doodle::computer> l_computers{};
  for (auto &&[l_e, l_c] : g_reg()->view<doodle::computer>().each()) {
    l_computers.emplace_back(l_c);
  }
  nlohmann::json l_json = l_computers;
  auto &l_req           = in_handle.get<http_session_data>().request_parser_->get();
  boost::beast::http::response<boost::beast::http::string_body> l_response{
      boost::beast::http::status::ok, l_req.version()
  };
  l_response.result(boost::beast::http::status::ok);
  l_response.keep_alive(l_req.keep_alive());
  l_response.set(boost::beast::http::field::content_type, "application/json");
  l_response.body() = l_json.dump();
  l_response.prepare_payload();
  in_handle.get<http_session_data>().seed(std::move(l_response));
}
void computer::reg_computer(boost::system::error_code in_error_code, const entt::handle in_handle) {
  auto l_logger = in_handle.get<socket_logger>().logger_;
  auto l_remote_endpoint =
      boost::beast::get_lowest_layer(*in_handle.get<http_websocket_data>().stream_).socket().remote_endpoint();
  l_logger->log(log_loc(), level::info, "注册计算机 {}", l_remote_endpoint.address().to_string());
  in_handle.emplace<doodle::computer>("", l_remote_endpoint.address().to_string()).server_status_ =
      doodle::computer_status::free;
  in_handle.get<http_websocket_data>().on_message.connect(&computer::websocket_route);
}

void computer::reg(doodle::http::http_route &in_route) {
  in_route.reg(std::make_shared<http_function>(
      boost::beast::http::verb ::get, "v1/computer",
      session::make_http_reg_fun(
          boost::asio::bind_executor(g_io_context(), &computer::list_computers),
          boost::asio::bind_executor(g_io_context(), &computer::reg_computer)
      )
  ));
}
}  // namespace doodle::http