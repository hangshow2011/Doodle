//
// Created by TD on 2022/5/9.
//

#include "session.h"
#include <boost/asio.hpp>
#include <doodle_core/json_rpc/core/parser_rpc.h>
#include <doodle_core/json_rpc/core/rpc_server.h>
#include <boost/asio/spawn.hpp>

#include <string>

namespace doodle {
namespace json_rpc {

class parser_rpc;
class rpc_server;
class rpc_server_ref;
class session_manager;
class session::impl {
 public:
  explicit impl(
      boost::asio::io_context& in_io_context,
      boost::asio::ip::tcp::socket in_socket)
      : io_context_(in_io_context),
        socket_(std::move(in_socket)),
        data_(),
        rpc_server_(),
        parser_rpc_(){

        };
  boost::asio::io_context& io_context_;
  boost::asio::ip::tcp::socket socket_;
  parser_rpc parser_rpc_;
  std::shared_ptr<rpc_server_ref> rpc_server_;

  boost::asio::streambuf data_{};
  std::string msg_{};

  bool stop_{false};
};

session::session(boost::asio::io_context& in_io_context,
                 boost::asio::ip::tcp::socket in_socket)
    : ptr(std::make_unique<impl>(in_io_context, std::move(in_socket))){

      };

void session::start(std::shared_ptr<rpc_server_ref> in_server) {
  ptr->rpc_server_ = std::move(in_server);
  boost::asio::spawn(ptr->io_context_,
                     [self = shared_from_this(), this](const boost::asio::yield_context& yield) {
                       boost::signals2::signal<void(const std::string&)> l_sig{};
                       l_sig.connect([&](const std::string& in_string) {
                         if (!ptr->socket_.is_open())
                           return;

                         boost::asio::async_write(ptr->socket_,
                                                  boost::asio::buffer(in_string + division_string),
                                                  yield);
                       });
                       while (!ptr->stop_) {
                         boost::system::error_code ec{};
                         using iter_buff = boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type>;

                         std::function<
                             std::pair<iter_buff, bool>(iter_buff in_begen, iter_buff in_end)>
                             l_function{
                                 [](iter_buff in_begin, const iter_buff& in_end) -> std::pair<iter_buff, bool> {
                                   iter_buff i = std::move(in_begin);
                                   while (i != in_end)
                                     if (std::isspace(*i++))
                                       return std::make_pair(i, true);
                                   return std::make_pair(i, false);
                                 }};
                         boost::asio::async_read_until(
                             ptr->socket_,
                             ptr->data_,
                             l_function,
                             yield[ec]);
                         if (!ec) {
                           std::istream l_istream{&ptr->data_};
                           std::string l_ine{};
                           std::getline(l_istream, l_ine);
                           if (l_ine.empty())
                             continue;

                           ptr->parser_rpc_.json_data_attr(l_ine);
                           ptr->parser_rpc_(l_sig, *ptr->rpc_server_);

                           std::string end{end_string};
                           if (ptr->socket_.is_open())
                             boost::asio::async_write(ptr->socket_,
                                                      boost::asio::buffer(end),
                                                      yield[ec]);
                         } else {
                           ptr->rpc_server_->close_current();
                           break;
                         }
                       }
                     });
}

void session::stop() {
  ptr->socket_.close();
  ptr->stop_ = true;
}
session::~session() = default;

}  // namespace json_rpc
}  // namespace doodle
