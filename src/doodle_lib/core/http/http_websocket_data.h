//
// Created by TD on 2024/2/20.
//

#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/url.hpp>
namespace doodle::http {
class http_websocket_data {
 public:
  using websocket_stream = boost::beast::websocket::stream<boost::beast::tcp_stream>;
  explicit http_websocket_data(boost::beast::tcp_stream in_stream)
      : stream_(std::make_unique<websocket_stream>(std::move(in_stream))) {}

  std::unique_ptr<websocket_stream> stream_;
  boost::beast::flat_buffer buffer_{};  // (Must persist between reads)
};

}  // namespace doodle::http