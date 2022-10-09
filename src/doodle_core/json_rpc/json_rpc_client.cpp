//
// Created by TD on 2022/5/9.
//

#include "json_rpc_client.h"

#include <json_rpc/json_rpc_static_value.h>
#include <json_rpc/args/rpc_json_progress.h>
#include <metadata/move_create.h>
#include <doodle_core/lib_warp/entt_warp.h>
#include <metadata/project.h>

namespace doodle {

json_rpc_client::json_rpc_client(boost::asio::io_context& in_context, const std::string& in_host, uint16_t in_post)
    : rpc_client(in_context, in_host, in_post) {}
void json_rpc_client::image_to_move(const std::vector<movie::image_attr>& in_list) {
  call_fun<void, false>(json_rpc::rpc_fun_name::image_to_move, in_list);
}
process_message json_rpc_client::get_progress(entt::entity in_id) {
  return call_fun<process_message>(json_rpc::rpc_fun_name::get_progress, in_id);
}
void json_rpc_client::stop_app() {
  return call_fun<void, true>(json_rpc::rpc_fun_name::stop_app);
}

}  // namespace doodle
