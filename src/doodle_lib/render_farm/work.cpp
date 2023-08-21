//
// Created by td_main on 2023/8/21.
//

#include "work.h"

#include <boost/beast.hpp>
namespace doodle {
namespace render_farm {
void work::on_wait(boost::system::error_code ec) {
  if (ec == boost::asio::error::operation_aborted) {
    return;
  }
  if (ec) {
    DOODLE_LOG_INFO("{}", ec.message());
    return;
  }
  run();
}
void work::do_wait() {
  ptr_->timer_->expires_after(doodle::chrono::seconds{2});

  ptr_->timer_->async_wait(std::bind(&work::on_wait, this, std::placeholders::_1));
}

void work::run() {
  auto l_s = boost::asio::make_strand(g_io_context());

  if (!ptr_->resolver_) {
    ptr_->resolver_ = std::make_shared<resolver>(l_s);
  }
  if (!ptr_->socket_) {
    ptr_->socket_ = std::make_shared<socket>(l_s);
  }
  if (!ptr_->timer_) {
    ptr_->timer_ = std::make_shared<timer>(l_s);
  }
  if (!ptr_->socket_->socket().is_open()) {
    ptr_->resolver_->async_resolve(
        ptr_->server_ip, "50021", std::bind(&work::on_resolve, this, std::placeholders::_1, std::placeholders::_2)
    );
  } else {
    DOODLE_LOG_INFO("socket is open");
    do_register();
  }
}

void work::on_resolve(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
  if (ec) {
    DOODLE_LOG_INFO("{}", ec.message());
    do_wait();
    return;
  }
  boost::asio::async_connect(ptr_->socket_->socket(), results, [this](auto&& PH1, auto&& PH2) {
    on_connect(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });
}

void work::on_connect(boost::system::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
  if (ec) {
    DOODLE_LOG_INFO("{}", ec.message());
    do_wait();
    return;
  }
  DOODLE_LOG_INFO("连接成功服务器");
  do_register();
}

void work::do_register() {
  ptr_->request_ = {boost::beast::http::verb::post, "/v1/render_farm/computer", 11};
  ptr_->request_.set(boost::beast::http::field::host, fmt::format("{}:50021", ptr_->server_ip));
  ptr_->request_.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  ptr_->request_.set(boost::beast::http::field::content_type, "application/json");
  ptr_->request_.set(boost::beast::http::field::accept, "application/json");
  nlohmann::json l_json;
  l_json["name"]        = boost::asio::ip::host_name();
  ptr_->request_.body() = l_json.dump();
  ptr_->request_.prepare_payload();
  boost::beast::http::async_write(*ptr_->socket_, ptr_->request_, [this](auto&& PH1, auto&& PH2) {
    on_write(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });
}
void work::on_write(boost::system::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);
  if (ec) {
    DOODLE_LOG_INFO("{}", ec.message());
    do_close();

    return;
  }
  do_read();
}
void work::do_read() {
  DOODLE_LOG_INFO("开始读取");
  boost::beast::http::async_read(*ptr_->socket_, ptr_->buffer_, ptr_->response_, [this](auto&& PH1, auto&& PH2) {
    on_read(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
  });
}
void work::on_read(boost::system::error_code ec, std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec) {
    DOODLE_LOG_INFO("{}", ec.message());
    do_close();
    return;
  }
  DOODLE_LOG_INFO("{}", ptr_->response_.body());
  ptr_->buffer_.clear();
  ptr_->response_ = {};
  do_wait();
}
void work::do_close() { ptr_->socket_->socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send); }
}  // namespace render_farm
}  // namespace doodle