//
// Created by td_main on 2023/8/21.
//
#include <doodle_lib/doodle_lib_fwd.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace doodle {
namespace render_farm {

class work {
 public:
  using timer        = boost::asio::system_timer;
  using timer_ptr    = std::shared_ptr<timer>;
  using socket        = boost::beast::tcp_stream;
  using socket_ptr   = std::shared_ptr<socket>;
  using resolver     = boost::asio::ip::tcp::resolver;
  using resolver_ptr = std::shared_ptr<resolver>;

  using buffer_type   = boost::beast::flat_buffer;

  using response_type = boost::beast::http::response<boost::beast::http::string_body>;

 private:
  struct data_type {
    std::string server_ip{};
    timer_ptr timer_{};
    socket_ptr socket_{};
    resolver_ptr resolver_{};
    // buffer
    buffer_type buffer_{};
    // requset
    response_type response_{};
  };
  std::shared_ptr<data_type> ptr_;

 public:
  explicit work(std::string in_server_ip) : ptr_{std::make_shared<data_type>()} {
    ptr_->server_ip = std::move(in_server_ip);
  }

  ~work() = default;

  void run();

 private:
  void do_register();
  // on_resolve
  void on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

  // on_connect
  void on_connect(boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint);
  // on_write
  void on_write(boost::system::error_code ec, std::size_t bytes_transferred);
  // on_read
  void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
};

}  // namespace render_farm
}  // namespace doodle
