//
// Created by TD on 2022/4/29.
//

#pragma once

#include <nlohmann/json.hpp>

#include <doodle_core/json_rpc/core/parser_rpc.h>
#include <doodle_core/json_rpc/core/rpc_request.h>

#include <boost/coroutine2/coroutine.hpp>
namespace boost::asio {
class io_context;
}

namespace doodle::json_rpc {

class rpc_client {
  class impl;
  std::unique_ptr<impl> ptr;

 public:
  using string_coroutine = boost::coroutines2::coroutine<std::string>;
  explicit rpc_client(boost::asio::io_context& in_context,
                      const std::string& in_host,
                      std::uint16_t in_post);
  ~rpc_client();

 protected:
  std::string call_server(const std::string& in_string, bool is_notice);
  void call_server(const std::string& in_string,
                   string_coroutine::push_type& in_skin);

  template <typename Result_Type,
            typename Arg,
            std::enable_if_t<!std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  auto call_fun(const std::string& in_name, Arg args) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = false;
    l_rpc_request.params_   = args;

    l_json                  = l_rpc_request;
    std::string l_json_str{};
    l_json_str         = call_server(l_json.dump(), false);

    nlohmann::json l_r = nlohmann::json::parse(l_json_str);
    auto l_rpc_r       = l_r.template get<rpc_reply>();
    if (l_rpc_r.result.index() != rpc_reply::err_index) {
      return std::get<nlohmann::json>(l_rpc_r.result).template get<Result_Type>();
    } else {
      auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
      l_err_.to_throw();
    }
  }

  template <typename Result_Type,
            std::enable_if_t<!std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  auto call_fun(const std::string& in_name) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = false;

    l_json                  = l_rpc_request;
    std::string l_json_str{};
    l_json_str         = call_server(l_json.dump(), false);

    nlohmann::json l_r = nlohmann::json::parse(l_json_str);
    auto l_rpc_r       = l_r.template get<rpc_reply>();
    if (l_rpc_r.result.index() == rpc_reply::err_index) {
      auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
      l_err_.to_throw();
    }
  }

  template <typename Result_Type,
            bool is_notice_type,
            typename Arg,
            std::enable_if_t<std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  auto call_fun(const std::string& in_name, Arg args) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = is_notice_type;
    l_rpc_request.params_   = args;

    l_json                  = l_rpc_request;
    std::string l_json_str{};
    if constexpr (is_notice_type) {
      call_server(l_json.dump(), is_notice_type);
      return;
    } else {
      l_json_str = call_server(l_json.dump(), is_notice_type);
    }

    nlohmann::json l_r = nlohmann::json::parse(l_json_str);
    auto l_rpc_r       = l_r.template get<rpc_reply>();
    if (l_rpc_r.result.index() == rpc_reply::err_index) {
      auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
      l_err_.to_throw();
    }
  }

  template <typename Result_Type,
            bool is_notice_type,
            std::enable_if_t<std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  auto call_fun(const std::string& in_name) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = is_notice_type;

    l_json                  = l_rpc_request;
    std::string l_json_str{};
    if constexpr (is_notice_type) {
      call_server(l_json.dump(), is_notice_type);
      return;
    } else {
      l_json_str = call_server(l_json.dump(), is_notice_type);
    }

    nlohmann::json l_r = nlohmann::json::parse(l_json_str);
    auto l_rpc_r       = l_r.template get<rpc_reply>();
    if (l_rpc_r.result.index() == rpc_reply::err_index) {
      auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
      l_err_.to_throw();
    }
  }

  template <typename Result_Type,
            typename Arg, std::enable_if_t<!std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  void call_fun(const std::string& in_name,
                typename boost::coroutines2::coroutine<Result_Type>::push_type& in_skin,
                Arg args) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = false;
    l_rpc_request.params_   = args;

    l_json                  = l_rpc_request;

    string_coroutine::pull_type l_pull_type{[this, &in_name](string_coroutine::push_type& in_str_skin) {
      return call_server(in_name, in_str_skin);
    }};

    for (auto&& l_json_str : l_pull_type) {
      nlohmann::json l_r = nlohmann::json::parse(l_json_str);
      auto l_rpc_r       = l_r.template get<rpc_reply>();
      if (l_rpc_r.result.index() != rpc_reply::err_index) {
        in_skin(std::get<nlohmann::json>(l_rpc_r.result).template get<Result_Type>());
      } else {
        auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
        l_err_.to_throw();
      }
    }
  }

  template <typename Result_Type, std::enable_if_t<!std::is_same_v<void, Result_Type>, std::int32_t> = 0>
  void call_fun(const std::string& in_name,
                typename boost::coroutines2::coroutine<Result_Type>::push_type& in_skin) {
    nlohmann::json l_json{};

    rpc_request l_rpc_request{};
    l_rpc_request.method_   = in_name;
    l_rpc_request.is_notice = false;

    l_json                  = l_rpc_request;

    string_coroutine::pull_type l_pull_type{[this, &in_name](string_coroutine::push_type& in_str_skin) {
      return call_server(in_name, in_str_skin);
    }};
    for (auto l_json_str : l_pull_type) {
      nlohmann::json l_r = nlohmann::json::parse(l_json_str);
      auto l_rpc_r       = l_r.template get<rpc_reply>();
      if (l_rpc_r.result.index() != rpc_reply::err_index) {
        in_skin(std::get<nlohmann::json>(l_rpc_r.result).template get<Result_Type>());
      } else {
        auto l_err_ = std::get<rpc_error>(l_rpc_r.result);
        l_err_.to_throw();
      }
    }
  }
  void close();
};
}  // namespace doodle::json_rpc
