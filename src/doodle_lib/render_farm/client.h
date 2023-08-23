//
// Created by td_main on 2023/8/18.
//
#pragma once
#include <doodle_lib/doodle_lib_fwd.h>
#include <doodle_lib/render_farm/detail/basic_json_body.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
namespace doodle {
class client {
 private:
  using timer_t                = boost::asio::system_timer;
  using timer_ptr              = std::shared_ptr<timer_t>;
  using socket_t               = boost::beast::tcp_stream;
  using socket_ptr             = std::shared_ptr<socket_t>;
  using resolver_t             = boost::asio::ip::tcp::resolver;
  using resolver_ptr           = std::shared_ptr<resolver_t>;
  using buffer_type            = boost::beast::flat_buffer;
  using response_type          = boost::beast::http::response<render_farm::detail::basic_json_body>;
  using request_type           = boost::beast::http::request<render_farm::detail::basic_json_body>;
  using message_generator_type = boost::beast::http::message_generator;
  using message_generator_ptr  = std::shared_ptr<message_generator_type>;

  struct data_type {
    std::string server_ip_;

    socket_ptr socket_{};
    timer_ptr timer_{};
    resolver_ptr resolver_{};

    buffer_type buffer_;
    response_type response_;
    message_generator_ptr request_;
    std::int32_t connect_count_{};

    std::queue<std::function<void()>> queue_;
    std::atomic_bool queue_running_{};
  };
  std::shared_ptr<data_type> ptr_;

  void make_ptr();

 public:
  struct queue_action_guard {
    data_type* ptr_;
    explicit queue_action_guard(data_type* in_ptr) : ptr_(in_ptr) {}
    ~queue_action_guard();
  };

  enum state {
    start,
    resolve,
    connect,
    write,
    read,
  };

 public:
  struct computer {
    std::int64_t id_{};
    std::string name_{};
    std::string state_{};
    // form json
    friend void from_json(const nlohmann::json& in_json, computer& out_data) {
      in_json["id"].get_to(out_data.id_);
      in_json["name"].get_to(out_data.name_);
      in_json["state"].get_to(out_data.state_);
    }
  };
  struct computer_list_t {
    using response_type = boost::beast::http::response<render_farm::detail::basic_json_body>;
    using result_type   = std::vector<computer>;
    result_type result_;
    client* ptr_;

    boost::beast::http::message_generator operator()() {
      boost::beast::http::request<render_farm::detail::basic_json_body> l_request{
          boost::beast::http::verb::get, "/v1/render_farm/render_job", 11};
      l_request.keep_alive(true);
      l_request.set(boost::beast::http::field::host, fmt::format("{}:50021", ptr_->server_ip()));
      l_request.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
      l_request.set(boost::beast::http::field::content_type, "application/json");
      l_request.set(boost::beast::http::field::accept, "application/json");
      return {std::move(l_request)};
    };

    result_type operator()(const response_type& in_response) { return in_response.body().get<std::vector<computer>>(); }
  };

  struct computer_list_t {};

  template <typename ExecutorType, typename CompletionHandler, typename ActionType>
  struct connect_op_type : boost::beast::async_base<std::decay_t<CompletionHandler>, ExecutorType>,
                           boost::asio::coroutine {
    using response_type = typename ActionType::response_type;
    using result_type   = typename ActionType::result_type;
    struct data_type2 {
      buffer_type buffer_;
      response_type response_;
      client* ptr_;
      state state_ = start;
      message_generator_ptr request_;
      ActionType action_;
      std::unique_ptr<client::queue_action_guard> guard_;
    };
    std::unique_ptr<data_type2> ptr_;

    using base_type = boost::beast::async_base<std::decay_t<CompletionHandler>, ExecutorType>;

    auto& socket() { return ptr_->ptr_->socket(); }
    auto& stream() { return ptr_->ptr_->stream(); }
    auto& resolver() { return ptr_->ptr_->resolver(); }
    client::data_type& client_data() { return *ptr_->ptr_->ptr_; }

    connect_op_type(
        client* in_ptr, CompletionHandler&& in_handler, const ExecutorType& in_executor_type_1, ActionType in_action
    )
        : base_type(std::move(in_handler), in_executor_type_1), ptr_(std::make_unique<data_type2>()) {
      ptr_->ptr_         = in_ptr;
      ptr_->action_      = std::move(in_action);
      ptr_->action_.ptr_ = in_ptr;
      ptr_->guard_       = std::make_unique<client::queue_action_guard>(in_ptr->ptr_.get());
    }
    ~connect_op_type()                                 = default;
    // move
    connect_op_type(connect_op_type&&)                 = default;
    connect_op_type& operator=(connect_op_type&&)      = default;
    // copy
    connect_op_type(const connect_op_type&)            = delete;
    connect_op_type& operator=(const connect_op_type&) = delete;

    void run() {
      client_data().queue_running_ = true;
      if (socket().is_open()) {
        do_write();
      } else {
        do_resolve();
      }
    }
    void operator()(boost::system::error_code ec, std::size_t bytes_transferred) {
      switch (ptr_->state_) {
        case state::write: {
          boost::ignore_unused(bytes_transferred);
          if (ec == boost::beast::errc::not_connected || ec == boost::beast::errc::connection_reset ||
              ec == boost::beast::errc::connection_refused || ec == boost::beast::errc::connection_aborted) {
            DOODLE_LOG_INFO("失去连接, 开始重新连接");
            do_resolve();
            return;
          }
          do_read();
          break;
        }

        case state::read: {
          boost::ignore_unused(bytes_transferred);
          if (ec) {
            DOODLE_LOG_INFO("{}", ec.message());
            return;
          }
          this->complete(false, ec, ptr_->action_(ptr_->response_));
          break;
        }
        default: {
          if (ec) {
            DOODLE_LOG_INFO("{}", ec.message());
            return;
          }
          break;
        }
      }
    }

    void operator()(boost::system::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
      if (ec) {
        DOODLE_LOG_INFO("{}", ec.message());
        return;
      }
      boost::asio::async_connect(socket(), results, std::move(*this));
    }
    void operator()(boost::system::error_code ec, const boost::asio::ip::tcp::endpoint& endpoint) {
      boost::ignore_unused(endpoint);
      if (ec) {
        DOODLE_LOG_INFO("{}", ec.message());
        return;
      }
      DOODLE_LOG_INFO("连接成功服务器");
      do_write();
    }

    void do_write() {
      ptr_->state_   = write;

      ptr_->request_ = std::make_shared<message_generator_type>(std::move(ptr_->action_()));
      boost::beast::async_write(stream(), *ptr_->request_, std::move(*this));
    }
    void do_read() {
      ptr_->state_ = read;
      ptr_->buffer_.clear();
      ptr_->response_ = {};
      boost::beast::http::async_read(stream(), ptr_->buffer_, ptr_->response_, std::move(*this));
    }
    void do_resolve() { resolver().async_resolve(client_data().server_ip_, "50021", std::move(*this)); }
  };

 public:
  explicit client(std::string in_server_ip) : ptr_(std::make_shared<data_type>()) {
    ptr_->server_ip_ = std::move(in_server_ip);
    make_ptr();
  }
  ~client() = default;

  // run
  void run();

  // server_ip
  [[nodiscard]] inline std::string& server_ip() { return ptr_->server_ip_; }
  [[nodiscard]] inline const std::string& server_ip() const { return ptr_->server_ip_; }
  inline void server_ip(std::string in_server_ip) { ptr_->server_ip_ = std::move(in_server_ip); }
  inline socket_t::socket_type& socket() { return ptr_->socket_->socket(); }
  inline socket_t& stream() { return *ptr_->socket_; }
  inline resolver_t& resolver() { return *ptr_->resolver_; }

  template <typename ExecutorType, typename CompletionHandler>
  auto async_connect(const ExecutorType& in_executor_type, CompletionHandler&& in_completion) {
    //    using async_completion =
    //        boost::asio::async_completion<CompletionHandler, void(boost::system::error_code, socket_ptr)>;
    //    using handler_type = typename async_completion::completion_handler_type;
    using connect_op = connect_op_type<ExecutorType, CompletionHandler>;
    return boost::asio::async_initiate<CompletionHandler, void(boost::system::error_code)>(
        [](auto&& in_completion_, client* in_client_ptr, const auto& in_executor_) {
          //          connect_op op{in_client_ptr, std::forward<decltype(in_completion_)>(in_completion_),
          //          in_executor_};
          auto l_h = std::make_shared<connect_op>(
              in_client_ptr, std::forward<decltype(in_completion_)>(in_completion_), in_executor_
          );
          auto& l_queue = in_client_ptr->ptr_->queue_;
          l_queue.emplace([l_self = l_h]() { l_self->run(); });
          if (!in_client_ptr->ptr_->queue_running_) l_queue.front()();
        },
        in_completion, this, in_executor_type
    );
  }

 public:
  template <typename CompletionHandler, typename ActionType>
  struct get_data_op : boost::beast::async_base<std::decay_t<CompletionHandler>, socket_t::executor_type>,
                       boost::asio::coroutine {
    struct data_type2 {
      buffer_type buffer_;
      boost::beast::http::response<boost::beast::http::string_body> response_;
      client* ptr_;
      state state_ = start;
      message_generator_ptr request_;
      ActionType action_;
      std::unique_ptr<client::queue_action_guard> guard_;
    };
    std::unique_ptr<data_type2> ptr_;
    explicit get_data_op(client* in_ptr, ActionType in_action, CompletionHandler&& in_handler)
        : boost::beast::async_base<std::decay_t<CompletionHandler>, socket_t::executor_type>(
              std::move(in_handler), in_ptr->stream().get_executor()
          ),
          ptr_(std::make_unique<data_type2>()) {
      ptr_->ptr_         = in_ptr;
      ptr_->guard_       = std::make_unique<client::queue_action_guard>(in_ptr->ptr_.get());
      ptr_->action_      = std::move(in_action);
      ptr_->action_.ptr_ = in_ptr;
    }
  };

  template <typename CompletionHandler>
  auto async_computer_list(CompletionHandler&& in_completion) {}

 private:
  void do_wait();
  // 获取渲染注册机器列表
  void do_get_computer_list();
  // 获取所有的渲染任务
  void do_get_task_list();

  //  void on_connect_timeout(boost::system::error_code ec);
};

}  // namespace doodle
