//
// Created by TD on 2022/5/17.
//
#include <catch2/catch_all.hpp>

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/app/app.h>
#include <doodle_lib/json_rpc/json_rpc_server.h>
#include <doodle_core/thread_pool/thread_pool.h>
#include <doodle_core/thread_pool/asio_pool.h>

#include <doodle_core/json_rpc/core/server.h>
#include <doodle_core/json_rpc/json_rpc_client.h>
#include <doodle_core/metadata/move_create.h>

using namespace doodle;
using namespace std::literals;
void test_client() {
  boost::asio::io_context io_context{};
  json_rpc_client l_c{io_context, "127.0.0.1"s, std::uint16_t{10223}};
  auto l_prj = l_c.open_project("D:/tmp/test2.doodle_db");
  std::cout << l_prj.p_path << std::endl;

  std::vector<movie::image_attr> l_vector{};

  json_rpc_client::image_to_move_sig l_sig{};

  l_sig.connect([](const json_rpc::args::rpc_json_progress& in_progress) {
    std::cout << in_progress.msg_ << std::endl;
  });

  l_c.image_to_move(l_sig, l_vector);
}

TEST_CASE("test json rpc") {
  auto l_app = app_command_base{};

  boost::asio::io_context l_context{};
  g_thread_pool().enqueue([&]() {
    json_rpc::server l_server{l_context, 10223};
    l_server.set_rpc_server(std::make_shared<json_rpc_server>());
    l_context.run();
  });

  g_pool().attach<one_process_t>(g_io_context(),[&]() {
    test_client();
    g_io_context().stop();
    l_context.stop();
    l_app.stop_app();
  });
  l_app.run();
}
