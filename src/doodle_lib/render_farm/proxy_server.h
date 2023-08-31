//
// Created by td_main on 2023/8/18.
//
#pragma once
#include <doodle_core/core/global_function.h>

#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/render_farm/client_core.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <memory>
namespace doodle {
class proxy_server {
 public:
  using stream_t     = boost::beast::tcp_stream;
  using resolver_t   = boost::asio::ip::tcp::resolver;
  using resolver_ptr = std::shared_ptr<resolver_t>;
  explicit proxy_server(
      boost::asio::io_context& in_io_context, std::uint16_t in_port, std::string in_server_address,
      std::string in_server_port
  )
      : end_point_{boost::asio::ip::tcp::v4(), in_port},
        acceptor_{in_io_context, end_point_},
        server_address_{std::move(in_server_address)},
        server_port_{std::move(in_server_port)} {}
  void run();
  void stop();

 private:
  friend class proxy_server_session;
  void do_accept();
  void on_accept(boost::system::error_code ec, boost::asio::ip::tcp::socket socket);

  void do_resolve();
  void do_connect();
  void do_connect_sync();

  boost::asio::ip::tcp::endpoint end_point_;
  boost::asio::ip::tcp::acceptor acceptor_;
  std::string server_address_;
  std::string server_port_{"50021"};

  std::shared_ptr<stream_t> server_stream_;
  resolver_ptr resolver_{};
  resolver_t::results_type resolver_results_;
};

}  // namespace doodle
