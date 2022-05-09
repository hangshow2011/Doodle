//
// Created by TD on 2022/4/29.
//

#include "rpc_client.h"
#include <json_rpc/core/server.h>
#include <boost/asio.hpp>
#include <json_rpc/core/rpc_reply.h>
#include <doodle_core/json_rpc/exception/json_rpc_error.h>
#include <json_rpc/core/session.h>

namespace doodle::json_rpc {

class rpc_client::impl {
 public:
  explicit impl(boost::asio::io_context &in_context)
      : client_socket(in_context){};
  boost::asio::ip::tcp::socket client_socket;
};

rpc_client::rpc_client(boost::asio::io_context &in_context,
                       const std::string &in_host, std::uint16_t in_post)
    : ptr(std::make_unique<impl>(in_context)) {
  ptr->client_socket.connect(boost::asio::ip::tcp::endpoint{
      boost::asio::ip::address::from_string(in_host),
      in_post});
};

std::string rpc_client::call_server(const std::string &in_string, bool is_notice) {
  boost::asio::write(ptr->client_socket, boost::asio::buffer(in_string + session::end_string));
  if (is_notice)
    return {};
  boost::asio::streambuf l_r{};
  auto l_size = boost::asio::read_until(ptr->client_socket, l_r, session::end_string);
  std::string l_str{
      boost::asio::buffers_begin(l_r.data()),
      boost::asio::buffers_begin(l_r.data()) + l_size - session::end_string.size()};
  return l_str;
}

void rpc_client::close() {
  return this->call_fun<true, void>("rpc.close"s);
}

rpc_client::~rpc_client() {
  close();
}
}  // namespace doodle::json_rpc
